# Messenger ConsoleTalk
## A messenger running in command line mode
### Using server
```
messenger_server <port>
```
- **port** - tcp port number for incoming connections.

### Using client
```
messenger_client <server> <port>
```
- **server** - ip address of message server.
- **port** - tcp port number for connection to server.

### List of commands
- **reg \<name> \<password>** - registration on the message server
- **login \<name> \<password>** - login on the message server
- **dial \<name>** - start a dialog with the user **\<name>**
- **[msg] \<message>** - send **\<message>** to a previously started dialog
- **file \<file_name>** - send file **\<file_name>** to a previously started dialog
- **exit** - exit
