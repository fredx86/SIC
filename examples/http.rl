CTL             = #0-31 | ~127
CRLF            = ~13 ~10
SP              = " "
HT              = ~9
HEX             = digit | `a` | `b` | `c` | `d` | `e` | `f`


ESCAPE          = "%" HEX HEX
RESERVED        = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+"
EXTRA           = "!" | "*" | "'" | "(" | ")" | ","
SAFE            = "$" | "-" | "_" | "."
UNSAFE		= 

CRLF            = ~13 ~10
METHOD          = "GET" | "HEAD" | "POST" | "PUT" | "DELETE" | "TRACE" | "OPTIONS" | "CONNECT"



_main_          = CTL