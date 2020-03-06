
# Changelog

**1.2.0:**
- Added support for disconnected mode
- Receiver
	- Added motor limiting so no values past +/-127 (avoids overflow)
	- Removed some extraneous lines from all damping functions
	- Restructured (streamlined) damping functions
	- Fixed bug where damping would not take effect when it should

[**1.1.1:**](https://github.com/noahpalumbo/BB-8/commit/6e8e3ae2f59b290598aa769f1258f2aadb74deea)
- Modularized code for QoL readability
- Controller
	- new .h and .cpp
	- Message Debug Print
	- Set Pin Modes
- Receiver
	- new .h and .cpp
	- Header Variables
	- Message Debug Print
	- Set Pin Modes
	- Positive Motor Damping
	- Negative Motor Damping
	- Zero Motor Damping

[**1.1.0:**](https://github.com/noahpalumbo/BB-8/commit/2d3722d28d72cebc5c408ae232ef2b82f266f365)

- Head rotation command transmission integrated in both base and
   controller
- Array message transmission and parsing fully functional
- Minor Optimization
	 - Removed extraneous code during message parsing on
   base receiving end
	- Removed extraneous code during message parsing on
   base receiving end 
 - TODOS: 	
	 - Reconfigure pin layout 	
	 - Retest nRF24l01 with 3V3 input and check for same smooth operation 	
	 - Add 10uF electrolytic cap and monitor any changes in firmware operation

**1.0.1:**
- Array messaging (sending) functional.
- No head rotation integration
