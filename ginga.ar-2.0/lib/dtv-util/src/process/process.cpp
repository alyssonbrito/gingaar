#include "process.h"
#include "../log.h"
#include <string.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#define strdup _strdup
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#endif

namespace util {

namespace fs = boost::filesystem;

Process::Process( const std::string &cmd ) {
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
}

Process::Process( const std::string &cmd, const Params &params ) {
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	parameters(params);
}

Process::Process( const std::string &cmd, const Environment &env ) {
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	_environment = env;
}

Process::Process( const std::string &cmd, const Params &params, const Environment &env ) {
	_pid = INVALID_PROCESS_ID;
	addParam(cmd);
	parameters(params);
	_environment = env;
}

Process::~Process() {
	LINFO("Process", "Destructor begin");
	clearParams();
	clearEnvironment();
	if (_pid!=INVALID_PROCESS_ID) {
		LWARN("Process", "killing process on destructor");
		kill();
	}
}

bool Process::run() {
	assert(_pid==INVALID_PROCESS_ID);
	bool result=false;

	if (fs::exists( _parameters[0] )) {
		std::string cmd="";
		BOOST_FOREACH(std::string param, _parameters) {
			cmd += param;
			cmd += " ";
		}
		LINFO("Process", "Spawn: %s", cmd.c_str());
		result = true;
	} else {
		LWARN("Process", "Spawn fails. Could not found file=%s", _parameters[0].c_str());
	}

	if (result) {
#ifdef _WIN32
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		std::string cmdLine;
		BOOST_FOREACH ( std::string param,  _parameters) {
			cmdLine += param;
			cmdLine += " ";
		}

		char** env = makeEnv();
		//	Start process. 
		result = ::CreateProcess(
			NULL,           // No module name (use command line)
			(char *)cmdLine.c_str(),// Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			env,		// Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure
		) == TRUE;

		freeParams(env);
		if (!result) {
			LERROR("Process", "CreateProcess failed (%d)", GetLastError());
		} else {
			//	Save process
			_pid = pi.hProcess;

			//	Close process and thread handles.
			CloseHandle( pi.hThread );
		}
#else
		//	Fork process
		_pid = ::fork();
		if (_pid == 0) {	// child
			//	Close all descriptors!!!
			int maxfd=::sysconf(_SC_OPEN_MAX);
			for(int fd=3; fd<maxfd; fd++) {
				close(fd);
			}

			char **params = makeParams();

			//	Replace process
			if (_environment.size()>0) {
				char** env = makeEnv();
				result=::execve( params[0], params, env ) >= 0;
				freeParams( env );
			}
			else {
				result=::execv( params[0], params ) >= 0;
			}
			freeParams( params );

			if (!result) {
				LERROR("Process", "cannot execute process: cmd=%s", _parameters[0].c_str());
			} else {
				LINFO("Process", "Running process: cmd=%s", _parameters[0].c_str());
			}
		} else if (_pid < 0) { // failed to fork
			LERROR("Process", "cannot fork process");
			result=false;
		} else {
			LINFO("Process", "Fork process ok: pid=%d", _pid);
			result=true;
		}
#endif
	}

	return result;
}

void Process::kill( int msTimeout/*=0*/ ) {
	if (_pid != INVALID_PROCESS_ID) {
		LINFO("Process", "Kill process: pid=%d, timeout=%d", _pid, msTimeout);

		bool exit=checkProcess(false);
		int transcurred = 0;
		while (!exit && msTimeout > transcurred) {
			boost::this_thread::sleep( boost::posix_time::milliseconds(250) );
			transcurred += 250;
			exit=checkProcess(false);
		}

		if (!exit) {
#ifdef _WIN32
			::TerminateProcess( _pid, 10000 );
#else
			// Send kill
			::kill( _pid, SIGKILL );
#endif
		}

		// Wait to stop
		checkProcess( true );
		LDEBUG("Process", "Process killed successfully: old_pid=%d", _pid);
		_pid=INVALID_PROCESS_ID;
	} else {
		LWARN("Process", "Attempt to kill not running process");
	}
}

PROCESS_ID Process::pid() const {
	return _pid;
}

bool Process::isRunning() {
	bool result=false;
	if (_pid != INVALID_PROCESS_ID) {
		result=!checkProcess(false);
		if (!result) {
			_pid = INVALID_PROCESS_ID;
		}
	}
	return result;
}

//	Params
void Process::parameters( const Params &params ) {
	_parameters.insert( _parameters.end(), params.begin(), params.end() );
}

void Process::addParam( const std::string &param ) {
	_parameters.push_back(param);
}

void Process::clearParams() {
	std::string cmd = _parameters[0];
	_parameters.clear();
	addParam(cmd);
}

//	Environments
void Process::environment( const std::string &env ) {
	const char *tmp = getenv( env.c_str() );
	if ( tmp ) {
		addToEnvironment("CMD_PROCESS_ENV", tmp);
	}
}

void Process::addToEnvironment( const std::string &var ) {
	std::vector<std::string> result;
	boost::algorithm::split(result, var, boost::algorithm::is_any_of("="));
	assert(result.size()==2);

	addToEnvironment(result[0], result[1]);
}

void Process::addToEnvironment( const std::string &key, const std::string &value ) {
	std::string var = _environment[key];
	if (!var.empty()) {
		var += PATH_SEPARATOR;
	}
	var += value;
	_environment[key] = var;
}

void Process::clearEnvironment() {
	_environment.clear();
}

bool Process::checkProcess( bool wait ) {
#ifdef _WIN32
	//	Wait until child process exits.
	bool result=WaitForSingleObject( _pid, wait ? INFINITE : 0 ) == WAIT_OBJECT_0;
	if (result && wait) {
		CloseHandle( _pid );
	}
	return result;
#else
	return ::waitpid( _pid, NULL, wait ? 0 : WNOHANG ) == _pid;
#endif
}

char **Process::makeParams() const {
	char **cmd = (char **)calloc( _parameters.size()+1, sizeof(char *) );

	int i=0;
	BOOST_FOREACH(std::string param, _parameters) {
		cmd[i] = strdup(param.c_str());
		i++;
	}
	cmd[i] = NULL;
	
	return cmd;
}

char** Process::makeEnv() const {
	char **cmd;

	int size = _environment.size();
	cmd = (char **)calloc( size+1, sizeof(char *) );

	int i=0;
	typedef std::pair<std::string,std::string> EnvironVar;
	BOOST_FOREACH(  EnvironVar var, _environment ) {
		std::string str;
		str += var.first.c_str();
		str += "=";
		str += var.second.c_str();
		cmd[i] = strdup(str.c_str());
		i++;
	}
	cmd[i] = NULL;

	return cmd;
}

void Process::freeParams( char **params ) {
	int i=0;
	if (params) {
		while (params[i]) {
			LDEBUG("Process", "Free param[%d] = '%s'", i, params[i]);
			free( params[i] );
			i++;
		}
		free( params );
	}
}

}
