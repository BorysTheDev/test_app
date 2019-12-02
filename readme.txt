I suppose that commands from client and response will be less than NamedPipe buffer (499 symbols).
For demonstration purposes, I've chosen "string" class and its methods.
Because I've chosen only one custom class I don't need to specify a type.

Architecture explanation:  
1) Asynchronous client use read and write callbacks functions: read callback start write operation and write callback start read operation;
2) Server use std::function with the vector of arguments to call specific object function or getting it attributes.

Howto:
1) Start server;
2) Start client;
3) Choose client mode.
4) You can create string object using the next command "create test_string", where "test_string" the name of your object;
5) List of commands:
assign new value: "call test_string assign test_value"
get substring: "call test_string substr 1 2" or "call test_string substr 1"
append string: "call test_string append _new"
getting string value: "call test_string"

