//
//  Vehicle.m
//  AnkiVehicleController
//
//  Created by Jeff on 4/24/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import "Vehicle.h"

@implementation Vehicle

-(id) initWithController:(id) controller
{
    self = [super init];
    _controller = controller;
    _active = false;
    
    return self;
}

-(bool) isActive {
    return _active;
}

-(void) setSpeed:(float) speed {
    anki_vehicle_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    uint8_t size = anki_vehicle_msg_set_speed(&msg, speed, 25000);
    [self sendMessage:&msg withLength:size];
}

-(void) spin180 {
    anki_vehicle_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    uint8_t size = anki_vehicle_msg_turn_180(&msg);
    [self sendMessage:&msg withLength:size];
}

-(void) sendMessage:(void*) buffer withLength:(uint8_t)len {
    NSData * valData = [NSData dataWithBytes:buffer length:len];
    [_peripheral writeValue:valData forCharacteristic:_writeChannel type:CBCharacteristicWriteWithoutResponse];
}


/*
 Invoked upon completion of a -[discoverServices:] request.
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    if (error)
    {
        NSLog(@"Discovered services for %@ with error: %@", peripheral.name, [error localizedDescription]);
        return;
    }
    assert(peripheral.services.count == 1);
    CBService *service = (CBService*)[peripheral.services objectAtIndex:0];
    NSLog(@"Discovered service, Id=%@", [service.UUID UUIDString]);
    [peripheral discoverCharacteristics:nil forService:service];
}

/*
 Invoked upon completion of a -[discoverCharacteristics:forService:] request.
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Discovered characteristics for %@ with error: %@", service.UUID, [error localizedDescription]);
        return;
    }
    
    for (CBCharacteristic * characteristic in service.characteristics)
    {
        if([characteristic.UUID isEqual:[CBUUID UUIDWithString:@ANKI_STR_CHR_WRITE_UUID]])
        {
            _writeChannel = characteristic;
            NSLog(@"Discovered write channel");
        }
        if([characteristic.UUID isEqual:[CBUUID UUIDWithString:@ANKI_STR_CHR_READ_UUID]])
        {
            _readChannel = characteristic;
            NSLog(@"Discovered read channel");
        }
    }
    
    [_peripheral setNotifyValue:YES forCharacteristic:_readChannel];
    
    anki_vehicle_msg_t msg;
    uint8_t size = 0;
    memset(&msg, 0, sizeof(msg));
    size = anki_vehicle_msg_set_sdk_mode(&msg, 1, ANKI_VEHICLE_SDK_OPTION_OVERRIDE_LOCALIZATION);
    [self sendMessage:&msg withLength:size];
    
    memset(&msg, 0, sizeof(msg));
    anki_vehicle_msg_get_version(&msg);
    [self sendMessage:&msg withLength:size];
    
    _active = true;
}

/*
 Invoked upon completion of a -[readValueForCharacteristic:] request or on the reception of a notification/indication.
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error updating value for characteristic %@ error: %@", characteristic.UUID, [error localizedDescription]);
        return;
    }
    
    anki_vehicle_msg_t msg;

    [characteristic.value getBytes:&msg length:sizeof(anki_vehicle_msg_t)];
    
    switch (msg.msg_id) {
        case ANKI_VEHICLE_MSG_V2C_PING_RESPONSE:
            NSLog(@"Ping recieved from vehicle");
            break;
        case ANKI_VEHICLE_MSG_V2C_VERSION_RESPONSE:
        {
            anki_vehicle_msg_version_response_t versionMsg;
            [characteristic.value getBytes:&versionMsg length:sizeof(anki_vehicle_msg_version_response_t)];
            NSLog(@"Version response 0x%04x\n", versionMsg.version);
            break;
        }
        case ANKI_VEHICLE_MSG_V2C_LOCALIZATION_POSITION_UPDATE:
        {
            anki_vehicle_msg_localization_position_update_t updateMsg;
            [characteristic.value getBytes:&updateMsg length:sizeof(anki_vehicle_msg_localization_position_update_t)];
            break;
        }
        case ANKI_VEHICLE_MSG_V2C_LOCALIZATION_TRANSITION_UPDATE:
        {
            anki_vehicle_msg_localization_transition_update_t transitionMsg;
            [characteristic.value getBytes:&transitionMsg length:sizeof(anki_vehicle_msg_localization_transition_update_t)];
            break;
        }
        case ANKI_VEHICLE_MSG_V2C_VEHICLE_DELOCALIZED:
        {
            NSLog(@"Warning: vehicle delocalized id=0x%04x", _identifier);
            break;
        }
        case ANKI_VEHICLE_MSG_V2C_BATTERY_LEVEL_RESPONSE:
        {
            break;
        }
        default:
            NSLog(@"unknown message recieved - 0x%04X, size %d", msg.msg_id, msg.size);
    }
}

/*
 Invoked upon completion of a -[writeValue:forCharacteristic:] request.
 */
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error writing value for characteristic %@ error: %@", characteristic.UUID, [error localizedDescription]);
        return;
    }
}

/*
 Invoked upon completion of a -[setNotifyValue:forCharacteristic:] request.
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error updating notification state for characteristic %@ error: %@", characteristic.UUID, [error localizedDescription]);
        return;
    }
}


@end
