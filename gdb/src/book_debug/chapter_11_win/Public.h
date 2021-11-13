/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_Helloworld,
    0xd92cff52,0xdfd3,0x49c2,0xa2,0x16,0xa1,0x6b,0x9e,0xde,0x3f,0xa9);
// {d92cff52-dfd3-49c2-a216-a16b9ede3fa9}
