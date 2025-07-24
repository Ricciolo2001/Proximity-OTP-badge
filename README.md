# Proximity OTP badge
## Abstract
We have multiple devices scattered around in the space defined as infrastructure devices and some clients that wants to interact with those ones.  
The infrastructure devices advertise their position constantly or at an interval depending on power settings and the client will scan for every advertising device.  
On connection with one of ths infrastructure devices the clien will autenticate himself using a secure connection using a given identity present inside a pre-generated file contained inside the devices

### Tipical uses
This is a new type of active badge protocol developed to enshure maximum securety in critical infrastructure accesses, it uses a 2.4GHz antenna to determine the distance from any device, in this way it can guide an user to the designated point of interest, avoiding confusion on what point of interest an user wants to interact with. 

The secrecy of the comunication is granted by the use of DH key exchange method and the auth is done using an otp generator.

To fully secure the legittimacy of the code sent the OTP wil be presented to the user, that, after reading it, it is required to autenticate iself by applyng a transformation to the OTP generated (a secret that only the designated badge knows) and, in case of success, the access to the point will be granted. *(not yet implemented)*

The idea was to use this protocol for farm equipment with the idea to protect expensive equipment from theft and from loosing smaller items in the field, but, thanks to the BLE communication that features really low power consumption, it might be used in any battery powered application and in a more portable badge form making it really versatile for exchanging small packets of data containing identification messages.
> Some significative examples are: managing access control to buildings, active badge tracking of users or advanced parcel localization in wharehouses and shipping containers.


## The protocol:
I have developed this protocol using the key exchange protocol Diffie-Hellman or DH in short form, this granted the creation of a secret key k wich is used by the two entities to exchange informations.

After obtaining the key we proceed to create a random number using a PRNG initialized with the same seed on both sides (the id and seeds are pregenerated and shared with all infrastructure items anc will be modifiable by some ids). To avoid managing multiple ids we have only one id and seed and the round of the seed is decided by the infrastructure device(this last feature is not implemented yet)

The generated key and otp are used to inzialize AES-CFB that will enshure complete encryption of the communication between the two devices and will mask patterns in repeated messages (the protocol will probably be used to send lots of similar small messages, so the CFB mode was the best)
Moreover the otp is always masked by the AES-CFB block encryption and is never shared, so an attacker will never be able to use it to get autentication on another device.

To get autenticated, the client sends a message containing his name and an aes encrypted message that contains a challenge(TRNG number) and his name.   
In this way if the decryption is successfull and the name extracted is equal to the name sent by the client is autenticated for the server.

After this the server adds one to the challenge and sends it back after encryption. If the client recieves the correct value after decryption, the autentication is complete.

<img src="Diagramma.png" > 


## Implementation
To implement this protocol i have used two ESP32s.
The device that represent the active badge and wants to connect to the infrastructure will sens all devices in the area with their relative distance.  
Sensed devices wil be displayaed by an oled display and will be updated. (having only 3 lines we will display only the closest devices (higher RSSI strenght))


<img src="ScannerFoto.jpg" > 

The infrastructure reciever is just a simle toggle switch for a light.