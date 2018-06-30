#include "pem_log_config.h"


/*!
 *
 *
 *
 */
static char* trim(char* str) {
	size_t i = strlen(str);

	while (i > 0 && isspace(str[i - 1])) {
		i--;
	}

	str[i] = '\0';

	i = strspn(str, "\t\n\v ");
	memmove(str, str + i, strlen(str + i) + 1);
	return str;
}


/*!
 *
 *
 */
static int toValidLogLevel(char* value) {
	if (value == 0) {
		PRINT_CONFIG_MSG("[%s::%d] Log Level is empty. Using default...\n", __FUNCTION__, __LINE__);
		return LOG_INVALID;
	}

	if      (strcasecmp(value, "LOG_FATAL" ) == 0)	{	return LOG_FATAL;	}
	else if (strcasecmp(value, "LOG_ERROR" ) == 0)	{	return LOG_ERROR;	}
	else if (strcasecmp(value, "LOG_WARN"  ) == 0)	{	return LOG_WARN;	}
	else if (strcasecmp(value, "LOG_INFO"  ) == 0)	{	return LOG_INFO;	}
	else if (strcasecmp(value, "LOG_DEBUG" ) == 0)	{	return LOG_DEBUG;	}

	PRINT_CONFIG_MSG("[%s::%d] Invalid Log Level. Using default...\n", __FUNCTION__, __LINE__);
	return LOG_INVALID;
}


/*!
 *
 * Limitations:
 * - 'stat' function does not work for files larger than 2GB on all systems
 *
 */
static int isFileExist(const char* filename) {
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}


/*!
 *
 *
 *
 */
static int initConfig(LauncherConfiguration *launcherConfig) {
	if (launcherConfig == 0) {
		return 0;
	}

	launcherConfig->enableLogLauncher	= 0;
	launcherConfig->enableLogEngine		= 0;
	launcherConfig->logLevel			= LOG_FATAL;
	launcherConfig->enableColor			= 0;
	launcherConfig->logMask				= LC_PRINT;

	return 1;
}


/*!
 *
 *
 */
static int createAndSaveDefaultConfigFile(const char* filename, LauncherConfiguration *launcherConfig) {
	FILE* fp = 0;
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	if (initConfig(launcherConfig) == 0) {
		PRINT_ERROR_MSG("Launcher Config is invalid!\n");
		return 0;
	}

	if ((fp = fopen(filename, "w")) == 0) {
		PRINT_ERROR_MSG("%s could not be created. Check permissions or disk space.\n", filename);
		return 0;
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%c", timeinfo);

	fprintf(fp, "###############################################\n");
	fprintf(fp, "#\n");
	fprintf(fp, "#\n");
	fprintf(fp, "# File generated on %s\n", buffer);
	fprintf(fp, "#\n");
	fprintf(fp, "#\n");
	fprintf(fp, "###############################################\n\n");

	// launcherConfig.enableLogLauncher = IS_ENABLED(value);
	fprintf(fp, "#\n");
	fprintf(fp, "# Enable print messages from Launcher.\n");
	fprintf(fp, "#\n");
	fprintf(fp, "ENABLE_LAUNCHER_LOG=false\n");
	fprintf(fp, "\n");

	// launcherConfig.enableLogEngine = IS_ENABLED(value);
	fprintf(fp, "#\n");
	fprintf(fp, "# Enable print messages from Ginga.Ar. By default the filter would be:\n");
	fprintf(fp, "#     '--debug=all.Port.all'\n");
	fprintf(fp, "#\n");
	fprintf(fp, "# or: $ ./ginga.sh --debug=all.Port.all\n");
	fprintf(fp, "#\n");
	fprintf(fp, "ENABLE_ENGINE_LOG=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logLevel = (level != LOG_INVALID ? level : LOG_FATAL);
	fprintf(fp, "#\n");
	fprintf(fp, "# Log Mask. You have the following values:\n");
	fprintf(fp, "# - LOG_FATAL\n");
	fprintf(fp, "# - LOG_ERROR\n");
	fprintf(fp, "# - LOG_WARN\n");
	fprintf(fp, "# - LOG_INFO\n");
	fprintf(fp, "# - LOG_DEBUG\n");
	fprintf(fp, "#\n");
	fprintf(fp, "LEVEL_LOG=LOG_FATAL\n");
	fprintf(fp, "\n");

	// launcherConfig.enableColor = IS_ENABLED(value);
	fprintf(fp, "#\n");
	fprintf(fp, "# Enable color format in print messages\n");
	fprintf(fp, "#\n");
	fprintf(fp, "USE_COLOR=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_LAUNCHER;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use LAUNCHER mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "LAUNCHER_LOG=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_MWAGT;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use MW_AGENT mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "MW_AGENT_LOG=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_TVAGT;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use TV_AGENT mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "TV_AGENT_LOG=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_PRINT;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use DEFAULT(LC_PRINT) mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "DEFAULT_PRINT=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_BRIDGE;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use NBRIDGE(LC_BRIDGE) mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "NBRIDGE=false\n");
	fprintf(fp, "\n");

	// launcherConfig.logMask |= LC_OSA;
	fprintf(fp, "#\n");
	fprintf(fp, "# Use OSA mask in PEM Log\n");
	fprintf(fp, "#\n");
	fprintf(fp, "OSA_LOG=false\n");
	fprintf(fp, "\n");

	fprintf(fp, "# The last line of this file MUST finish with an empty line\n\n");

	fclose(fp);
	return 1;
}


/*!
 *
 *
 */
static int readConfigFile(const char* filename, LauncherConfiguration *launcherConfig) {
	FILE* fp = 0;
	char line[256];

	if ((fp = fopen(filename, "r")) == 0) {
		PRINT_ERROR_MSG("ERROR, Cannot open file.\n");
		return 0;
	}

	while (fgets(line, 256, fp) != 0) {
		int i = 0;
		/* Skiping spaces until end of line is found or any character is found */
		trim(line);

		/* Skip comments */
		if (line[i] == '#') {
			PRINT_CONFIG_MSG("Found initial of commentary, next line.\n");
		}
		else
		/* End of line reached */
		if (line[i] == '\n') {
			PRINT_CONFIG_MSG("Line empty, next line.\n");
		}
		else
		if (strlen(line) > 0) {
			char* key   = 0;
			char* value = 0;
			key = strtok(line, "=");
			if (key == 0) {
				PRINT_CONFIG_MSG("ERROR, key not found. Did you forget it?\n");
			}
			else {
				value = strtok(0, "=");
				if (value == 0) {
					 PRINT_CONFIG_MSG("ERROR, key found but its value does not. Did you forget it?\n");
				}
				else {
					trim(key);
					trim(value);

					if (strcasecmp(key, "ENABLE_LAUNCHER_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("General Launcher Log", value);
						launcherConfig->enableLogLauncher = IS_ENABLED(value);
					}
					else if (strcasecmp(key, "ENABLE_ENGINE_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("Ginga Engine Log", value);
						launcherConfig->enableLogEngine = IS_ENABLED(value);
					}
					else if (strcasecmp(key, "LEVEL_LOG") == 0) {
						int level = toValidLogLevel(value);
						PRINT_CONFIG_MSG("Log Level: [%s][0x%x]\n", value, level);
						launcherConfig->logLevel = (level != LOG_INVALID ? level : LOG_FATAL);
					}
					else if (strcasecmp(key, "USE_COLOR") == 0) {
						PRINT_CONFIG_KEY_VALUE("User Color Flag", value);
						launcherConfig->enableColor = IS_ENABLED(value);
					}
					else if (strcasecmp(key, "LAUNCHER_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("Launcher Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_LAUNCHER;
							PRINT_CONFIG_MSG("Mask set to LC_LAUNCHER\n");
						}
					}
					else if (strcasecmp(key, "MW_AGENT_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("Middleware Agent Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_MWAGT;
							PRINT_CONFIG_MSG("Mask set to LC_MWAGT\n");
						}
					}
					else if (strcasecmp(key, "TV_AGENT_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("TV Agent Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_TVAGT;
							PRINT_CONFIG_MSG("Mask set to LC_TVAGT\n");
						}
					}
					else if (strcasecmp(key, "DEFAULT_PRINT") == 0) {
						PRINT_CONFIG_KEY_VALUE("Default Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_PRINT;
							PRINT_CONFIG_MSG("Mask set to LC_PRINT\n");
						}
					}
					else if (strcasecmp(key, "NBRIDGE") == 0) {
						PRINT_CONFIG_KEY_VALUE("NBridge Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_BRIDGE;
							PRINT_CONFIG_MSG("Mask set to LC_BRIDGE\n");
						}
					}
					else if (strcasecmp(key, "OSA_LOG") == 0) {
						PRINT_CONFIG_KEY_VALUE("OSA Log", value);
						if ( IS_ENABLED(value) ) {
							launcherConfig->logMask |= LC_OSA;
							PRINT_CONFIG_MSG("Mask set to OSA_LOG\n");
						}
					}
					else {
						PRINT_CONFIG_MSG("Unknown [Key, Value] property\n");
						PRINT_CONFIG_KEY_VALUE("Key", key);
						PRINT_CONFIG_KEY_VALUE("Value", value);
					}


					PRINT_CONFIG_MSG("Done tokenizer.\n");
				}
			}
		}
		/* Line has length ZERO. Do not do anything with it.
		else {}
		*/
	}

	fclose(fp);
	return 1;
}


/*!
 *
 *
 *
 */
int loadConfigFile(const char* filename, LauncherConfiguration *launcherConfig) {
	int res = -1;

	if (isFileExist(filename)) {
		res = readConfigFile (filename, launcherConfig);
		PRINT_CONFIG_MSG("Loading configuration: [%s]\n", IS_SUCCESS(res));
	}
	else {
		res = createAndSaveDefaultConfigFile(filename, launcherConfig);
		PRINT_CONFIG_MSG("Creating new configuration: [%s]\n", IS_SUCCESS(res));
	}

	return res;
}

