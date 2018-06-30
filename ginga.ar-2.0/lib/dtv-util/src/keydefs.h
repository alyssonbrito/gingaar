/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#ifndef KEYDEFS_H
#define KEYDEFS_H

#include <set>

#define UTIL_KEY_LIST(fnc)														 \
               fnc( number_0          , "0"                 ,numeric_keys      ) \
               fnc( number_1          , "1"                 ,numeric_keys      ) \
               fnc( number_2          , "2"                 ,numeric_keys      ) \
               fnc( number_3          , "3"                 ,numeric_keys      ) \
               fnc( number_4          , "4"                 ,numeric_keys      ) \
               fnc( number_5          , "5"                 ,numeric_keys      ) \
               fnc( number_6          , "6"                 ,numeric_keys      ) \
               fnc( number_7          , "7"                 ,numeric_keys      ) \
               fnc( number_8          , "8"                 ,numeric_keys      ) \
               fnc( number_9          , "9"                 ,numeric_keys      ) \
               fnc( small_a           , "a"                 ,                  ) \
               fnc( small_b           , "b"                 ,                  ) \
               fnc( small_c           , "c"                 ,                  ) \
               fnc( small_d           , "d"                 ,                  ) \
               fnc( small_e           , "e"                 ,                  ) \
               fnc( small_f           , "f"                 ,                  ) \
               fnc( small_g           , "g"                 ,                  ) \
               fnc( small_h           , "h"                 ,                  ) \
               fnc( small_i           , "i"                 ,                  ) \
               fnc( small_j           , "j"                 ,                  ) \
               fnc( small_k           , "k"                 ,                  ) \
               fnc( small_l           , "l"                 ,                  ) \
               fnc( small_m           , "m"                 ,                  ) \
               fnc( small_n           , "n"                 ,                  ) \
               fnc( small_o           , "o"                 ,                  ) \
               fnc( small_p           , "p"                 ,                  ) \
               fnc( small_q           , "q"                 ,                  ) \
               fnc( small_r           , "r"                 ,                  ) \
               fnc( small_s           , "s"                 ,                  ) \
               fnc( small_t           , "t"                 ,                  ) \
               fnc( small_u           , "u"                 ,                  ) \
               fnc( small_v           , "v"                 ,                  ) \
               fnc( small_w           , "w"                 ,                  ) \
               fnc( small_x           , "x"                 ,                  ) \
               fnc( small_y           , "y"                 ,                  ) \
               fnc( small_z           , "z"                 ,                  ) \
               fnc( capital_a         , "A"                 ,                  ) \
               fnc( capital_b         , "B"                 ,                  ) \
               fnc( capital_c         , "C"                 ,                  ) \
               fnc( capital_d         , "D"                 ,                  ) \
               fnc( capital_e         , "E"                 ,                  ) \
               fnc( capital_f         , "F"                 ,                  ) \
               fnc( capital_g         , "G"                 ,                  ) \
               fnc( capital_h         , "H"                 ,                  ) \
               fnc( capital_i         , "I"                 ,                  ) \
               fnc( capital_j         , "J"                 ,                  ) \
               fnc( capital_k         , "K"                 ,                  ) \
               fnc( capital_l         , "L"                 ,                  ) \
               fnc( capital_m         , "M"                 ,                  ) \
               fnc( capital_n         , "N"                 ,                  ) \
               fnc( capital_o         , "O"                 ,                  ) \
               fnc( capital_p         , "P"                 ,                  ) \
               fnc( capital_q         , "Q"                 ,                  ) \
               fnc( capital_r         , "R"                 ,                  ) \
               fnc( capital_s         , "S"                 ,                  ) \
               fnc( capital_t         , "T"                 ,                  ) \
               fnc( capital_u         , "U"                 ,                  ) \
               fnc( capital_v         , "V"                 ,                  ) \
               fnc( capital_w         , "W"                 ,                  ) \
               fnc( capital_x         , "X"                 ,                  ) \
               fnc( capital_y         , "Y"                 ,                  ) \
               fnc( capital_z         , "Z"                 ,                  ) \
               fnc( page_down         , "PAGE_DOWN"         ,                  ) \
               fnc( page_up           , "PAGE_UP"           ,                  ) \
               fnc( f1                , "F1"                ,                  ) \
               fnc( f2                , "F2"                ,                  ) \
               fnc( f3                , "F3"                ,                  ) \
               fnc( f4                , "F4"                ,                  ) \
               fnc( f5                , "F5"                ,                  ) \
               fnc( f6                , "F6"                ,                  ) \
               fnc( f7                , "F7"                ,                  ) \
               fnc( f8                , "F8"                ,                  ) \
               fnc( f9                , "F9"                ,                  ) \
               fnc( f10               , "F10"               ,                  ) \
               fnc( f11               , "F11"               ,                  ) \
               fnc( f12               , "F12"               ,                  ) \
               fnc( plus_sign         , "+"                 ,                  ) \
               fnc( minus_sign        , "-"                 ,                  ) \
               fnc( asterisk          , "*"                 ,                  ) \
               fnc( number_sign       , "#"                 ,                  ) \
               fnc( period            , "."                 ,                  ) \
               fnc( super             , "SUPER"             ,                  ) \
               fnc( printscreen       , "PRINTSCREEN"       ,                  ) \
               fnc( menu              , "MENU"              ,                  ) \
               fnc( info              , "INFO"              ,                  ) \
               fnc( epg               , "EPG"               ,                  ) \
               fnc( cursor_down       , "CURSOR_DOWN"       ,cursor_keys       ) \
               fnc( cursor_left       , "CURSOR_LEFT"       ,cursor_keys       ) \
               fnc( cursor_right      , "CURSOR_RIGHT"      ,cursor_keys       ) \
               fnc( cursor_up         , "CURSOR_UP"         ,cursor_keys       ) \
               fnc( channel_down      , "CHANNEL_DOWN"      ,                  ) \
               fnc( channel_up        , "CHANNEL_UP"        ,                  ) \
               fnc( volume_down       , "VOLUME_DOWN"       ,                  ) \
               fnc( volume_up         , "VOLUME_UP"         ,                  ) \
               fnc( enter             , "ENTER"             ,selection_keys    ) \
               fnc( ok                , "OK"                ,selection_keys    ) \
               fnc( red               , "RED"               ,colored_keys      ) \
               fnc( green             , "GREEN"             ,colored_keys      ) \
               fnc( yellow            , "YELLOW"            ,colored_keys      ) \
               fnc( blue              , "BLUE"              ,colored_keys      ) \
               fnc( backspace         , "BACKSPACE"         ,selection_keys    ) \
               fnc( back              , "BACK"              ,selection_keys    ) \
               fnc( escape            , "ESCAPE"            ,                  ) \
               fnc( exit              , "EXIT"              ,selection_keys    ) \
               fnc( power             , "POWER"             ,                  ) \
               fnc( rewind            , "REWIND"            ,                  ) \
               fnc( stop              , "STOP"              ,                  ) \
               fnc( eject             , "EJECT"             ,                  ) \
               fnc( play              , "PLAY"              ,                  ) \
               fnc( record            , "RECORD"            ,                  ) \
               fnc( pause             , "PAUSE"             ,                  ) \
               fnc( greater_than_sign , ">"                 ,                  ) \
               fnc( less_than_sign    , "<"                 ,                  ) \
               fnc( tap               , "TAP"               ,                  ) \
               fnc( mute              , "MUTE"              ,                  ) \
               fnc( audio             , "AUDIO"             ,                  ) \
               fnc( channel           , "CHANNEL"           ,                  ) \
               fnc( favorites         , "FAVORITES"         ,                  ) \
               fnc( subtitle          , "SUBTITLE"          ,                  ) \
               fnc( aspect            , "ASPECT"            ,                  ) \
               fnc( ch34              , "CH34"              ,                  ) \
               fnc( video             , "VIDEO"             ,                  ) \

namespace util {
namespace key {

/**
* @brief Enumeration for key identification.
*/

#define DO_ENUM_KEY( c, n, g ) c,
enum type {
	null = 0,
	UTIL_KEY_LIST(DO_ENUM_KEY)
	LAST
};
#undef DO_ENUM_KEY

typedef std::set<type> Keys;

//	Returns key name
const char *getKeyName( type kValue );
type getKey( const char *keyName );
const char *getKeyReservation( type kValue );

}
}

#endif /* KEYDEFS_H */

