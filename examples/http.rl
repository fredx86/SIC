VALID_TKN_CHAR  = alnum | "!" | "#" | "$" | "%" | "&" | "'" | "*" | "+" | "-" | "." | "^" | "_" | "`" | "|" | "~"
TOKEN           = +VALID_TKN_CHAR
CRLF            = ~13 ~10
ABSOLUTE_URL    = "http://" TOKEN [ ":" num ]
URI             = "*" | ABSOLUTE_URL
METHOD          = "GET" | "HEAD" | "POST" | "PUT" | "DELETE" | "TRACE" | "OPTIONS" | "CONNECT"
HTTP_VERSION    = "HTTP/" num "." num
REQUEST         = METHOD " " URI " " HTTP_VERSION
HEADER          = TOKEN ":" $ TOKEN CRLF
HEADERS         = *HEADER

_main_          = REQUEST CRLF HEADERS CRLF