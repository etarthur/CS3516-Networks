Instructions on running:
Note I run on windows command promt and powershell and it works both places, therefore I assume it will work with mac and linux terminals
httpclient:
	To run the client simply open a command prompt or linux terminal in the directory of this assignment, and run:
	.\httpclient.py [--ttl] HOSTNAME PORT

where --ttl is optional and returns the RTT(relay trip time) of the request.
HOSTNAME is either a url to a server that uses HTTP, or an IP to a server. If it's a webserver include www, and the url may contain http:// or 
https://, as those are removed in the program
PORt is the port to connect to, if you don't know the specific port to connect to, use 80

httpserver:
	To run the server follow the same instruction as above but instead in the prompt/terminal enter:
	.\httpserver PORT

where PORT is the port number you wish to launch the server on. 
The server may not terminate properly in such cases the only way to fix is to close the terminal and reopen. Apologies about this but multithreading
with user input seems to not work as I intended. You can try to press ctrl+c as this terminates most of python but may not work, closing out of the command prompt
will close it properly and all sockets will be closed. 

