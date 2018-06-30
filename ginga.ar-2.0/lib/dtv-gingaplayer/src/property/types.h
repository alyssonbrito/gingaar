#pragma once

#include <util/types.h>
#include <string>

#define PROPERTY_LIST(fnc, fnc1) \
        fnc ( device )                     \
                                           \
        fnc ( left )                       \
        fnc ( right )                      \
        fnc ( top )                        \
        fnc ( bottom )                     \
        fnc ( width )                      \
        fnc ( height )                     \
        fnc ( size )                       \
        fnc ( location )                   \
        fnc ( bounds )                     \
        fnc ( zIndex )                     \
        fnc ( title )                      \
                                           \
        fnc ( region )                     \
        fnc ( explicitDur )                \
        fnc ( freeze )                     \
        fnc ( player )                     \
                                           \
        fnc ( focusIndex )                 \
        fnc ( focusBorderColor )           \
        fnc ( focusBorderWidth )           \
        fnc ( focusBorderTransparency )    \
        fnc ( focusSrc )                   \
        fnc ( focusSelSrc )                \
        fnc ( selBorderColor )             \
        fnc ( focus )                      \
        fnc ( focusSelected )              \
                                           \
        fnc ( moveLeft )                   \
        fnc ( moveRight )                  \
        fnc ( moveUp )                     \
        fnc ( moveDown )                   \
                                           \
        fnc ( transIn )                    \
        fnc ( transOut )                   \
                                           \
        fnc ( soundLevel )                 \
        fnc ( balanceLevel )               \
        fnc ( trebleLevel )                \
        fnc ( bassLevel )                  \
                                           \
        fnc ( backgroundColor )            \
        fnc ( rgbChromakey )               \
        fnc ( visible )                    \
        fnc ( opacity )                    \
        fnc ( fit )                        \
        fnc ( fill )                       \
        fnc ( hidden )                     \
        fnc ( meet )                       \
        fnc ( meetBest )                   \
        fnc ( slice )                      \
                                           \
        fnc ( scroll )                     \
        fnc ( fontStyle )                  \
        fnc ( fontColor )                  \
        fnc ( fontFamily )                 \
        fnc ( fontSize )                   \
        fnc ( fontVariant )                \
        fnc ( fontWeight )                 \
        fnc ( textAlign )                  \
                                           \
        fnc ( reusePlayer )                \
        fnc ( playerLife )                 \
                                           \
        fnc ( type  )                      \
        fnc ( subtype )                    \
        fnc ( direction )                  \
        fnc ( dur )                        \
        fnc ( startProgress )              \
        fnc ( endProgress )                \
        fnc ( fadeColor )                  \
        fnc ( horRepeat )                  \
        fnc ( vertRepeat )                 \
        fnc ( borderWidth )                \
        fnc ( borderColor )                \
                                           \
        fnc ( leftToRight )                \
        fnc ( topToBootom )                \
        fnc ( rectangle )                  \
        fnc ( diamond )                    \
        fnc ( clockwiseTwelve )            \
        fnc ( clockwiseThree )             \
        fnc ( clockwiseSix )               \
        fnc ( clockwiseNine )              \
        fnc ( topLeftHorizontal )          \
        fnc ( topLeftVertical )            \
        fnc ( topLeftDiagonal )            \
        fnc ( topRightDiagonal )           \
        fnc ( bottomRightDiagonal )        \
        fnc ( bottomLeftDiagonal )         \
        fnc ( crossfade )                  \
        fnc ( fadeToColor )                \
        fnc ( fadeFromColor )              \
        fnc ( id )                         \
        fnc ( src )                        \
        fnc ( descriptor )                 \
        fnc ( referInstance )              \
                                           \
        fnc ( plan )                       \
        fnc ( style )                      \
        fnc ( refer )                      \
                                           \
        fnc ( component )                  \
        fnc ( interface )                  \
                                           \
        fnc ( presentationEvent )          \
        fnc ( selectionEvent )             \
        fnc ( attributionEvent )           \
        fnc1( x_mainclass, "x-mainclass" ) \
        fnc1( x_classpath, "x-classpath" )

namespace player {
namespace property {
namespace type {

/**
* @brief Enumeration for properties identification.
*/
#define DO_ENUM_PROPERTY( p ) p,
#define DO_ENUM_PROPERTY1( p, p1 ) p,
enum Type {
	null = 0,
	PROPERTY_LIST(DO_ENUM_PROPERTY,DO_ENUM_PROPERTY1)
	LAST
};
#undef DO_ENUM_PROPERTY
#undef DO_ENUM_PROPERTY1

};

const char *getPropertyName( type::Type pValue );
type::Type getProperty( const char *propertyName );

}
}

