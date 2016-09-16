CTL             = #0-31 | ~127
CRLF            = ~13 ~10
SP              = " "
HT              = ~9
HEX             = digit | `a` | `b` | `c` | `d` | `e` | `f`

URI           = ( ABSOLUTE_URI | RELATIVE_URI ) [ "#" FRAGMENT ]
ABSOLUTE_URI  = SCHEME ":" *( UCHAR | RESERVED )
RELATIVE_URI  = NET_PATH | ABS_PATH | REL_PATH

NET_PATH      = "//" NET_LOC [ ABS_PATH ]
ABS_PATH      = "/" REL_PATH
REL_PATH      = [ PATH ] [ ";" PARAMS ] [ "?" QUERY ]

PATH          = FSEGMENT *( "/" SEGMENT )
FSEGMENT      = PCHAR *PCHAR
SEGMENT       = *PCHAR

PARAMS        = PARAM *( ";" PARAM )
PARAM         = *( PCHAR | "/" )

SCHEME        = ( alpha | digit | "+" | "-" | "." ) *( alpha | digit | "+" | "-" | "." )
NET_LOC       = *( PCHAR | ";" | "?" )
QUERY         = *( UCHAR | RESERVED )
FRAGMENT      = *( UCHAR | RESERVED )

PCHAR         = UCHAR | ":" | "@" | "&" | "=" | "+"
UCHAR         = UNRESERVED | ESCAPE
UNRESERVED    = alpha | digit | SAFE | EXTRA

ESCAPE          = "%" HEX HEX
RESERVED        = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+"
EXTRA           = "!" | "*" | "'" | "(" | ")" | ","
SAFE            = "$" | "-" | "_" | "."
UNSAFE		      = CTL | SP | ~34 | "#" | "%" | "<" | ">"

_SP             = SP
_METHOD         = "GET" | "HEAD" | "POST" | "PUT" | "DELETE" | "TRACE" | "OPTIONS" | "CONNECT"
_REQUEST_URI    = "*" | ABSOLUTE_URI | ABS_PATH
_HTTP_VERSION   = "HTTP/" num "." num

_REQUEST_LINE   = _METHOD:method _SP _REQUEST_URI:uri _SP _HTTP_VERSION:version

_main_          = _REQUEST_LINE