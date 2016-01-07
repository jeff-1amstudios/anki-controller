//
//  VehiclesController.h
//  AnkiVehicleController
//
//  Created by Jeff on 4/24/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IOBluetooth/IOBluetooth.h>

#import "anki_sdk/anki_util.h"
#import "anki_sdk/advertisement.h"
#import "anki_sdk/common.h"
#import "anki_sdk/eir.h"
#import "anki_sdk/protocol.h"
#import "anki_sdk/uuid.h"
#import "anki_sdk/vehicle_gatt_profile.h"

#import "Vehicle.h"

@interface VehiclesController : NSObject <CBCentralManagerDelegate>
{
    CBCentralManager *manager;
}
@property (retain) NSMutableArray *vehicles;

-(id) init;
-(void) beginScan;
-(void) stopScan;
-(Vehicle*) findVehicleAttachedToPeripheral:(CBPeripheral*)p;
-(Vehicle*) findVehicleById:(uint32_t) identifier;

-(void) vehicleWasDisconnected:(Vehicle *)v;

@end
