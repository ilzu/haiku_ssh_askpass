# Native ssh_askpass for Haiku

This is a quick and dirty native ssh_askpass implementation for Haiku. It supports Haiku's KeyStore service, though as it is currently not secure, using it is not recommended. 

Known issues:
- If one connects to Haiku host using telnet, the SSH password requests will pop up on the desktop.
- The password field is just a normal text control, so the password could be picked up with for example with a right hey command.
- The Haiku KayStore system is not secure, as there is no authentication to the password containers. 
