# Native ssh_askpass for Haiku

This is a quick and dirty native ssh_askpass implementation for Haiku. It supports Haiku's KeyStore service, though as it is currently not secure, using it is not recommended. 

## Known issues:
- If one connects to Haiku host using telnet, the SSH password requests will pop up on the desktop.
- The Haiku KeyStore system is not secure, as there is no authentication to the password containers. 

## Translations
|Language|Translator|
|--------|---------|
|Catalan (ca)|Davidmp|
|English (Australian) (en_au)|jt15s|
|French (fr)|Maxime Chretien|
|Friulian (fur)|tmtfx|
|German (de)|humdinger|
|Spanish (es)|cafeina|
|Spanish (Latin American) (es_419)|cafeina|
|Swedish (sv)|Fredrik Modéen|

Thanks for all translators!
