//
//  VehiclesController.m
//  AnkiVehicleController
//
//  Created by Jeff on 4/24/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import "VehiclesController.h"


@implementation VehiclesController

-(id) init {
    self = [super init];
    _vehicles = [NSMutableArray new];
    return self;
}

-(void) beginScan {
    manager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    [manager scanForPeripheralsWithServices:[NSArray arrayWithObject:[CBUUID UUIDWithString:@ANKI_STR_SERVICE_UUID]] options:nil];
}

-(void) stopScan {
    [manager stopScan];
}

-(Vehicle*) findVehicleAttachedToPeripheral:(CBPeripheral*) p {
    for (Vehicle *v in _vehicles) {
        if (v.peripheral == p)
            return v;
    }
    return nil;
}

-(Vehicle*) findVehicleById:(uint32_t) identifier {
    for (Vehicle *v in _vehicles) {
        if (v.identifier == identifier)
            return v;
    }
    //NSLog(@"Failed to find vehicle id=0x%08x", identifier);
    return nil;
}


- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{

}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    Vehicle *v = [self findVehicleAttachedToPeripheral:peripheral];
    if (v != nil) {
        NSLog(@"Connected to vehicle id=0x%08x", v.identifier);
        [peripheral discoverServices:nil];
    }
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    Vehicle *v = [self findVehicleAttachedToPeripheral:peripheral];
    if (v != nil) {
        NSLog(@"Disconnected vehicle id=0x%08x, error=%@", v.identifier, [error localizedDescription]);
        [v.peripheral setDelegate:nil];
        [_vehicles removeObject:v];
    }
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    Vehicle *v = [self findVehicleAttachedToPeripheral:peripheral];
    if (v != nil) {
        NSLog(@"Failed to connect to vehicle id=0x%08x, error=%@", v.identifier, [error localizedDescription]);
        [v.peripheral setDelegate:nil];
        [_vehicles removeObject:v];
    }
}


- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    
    NSData * mfgdata = advertisementData[@"kCBAdvDataManufacturerData"];

    anki_vehicle_adv_mfg_t mfg;
    int result = anki_vehicle_parse_mfg_data(mfgdata.bytes, mfgdata.length, &mfg);
    
    if (result == 0 && mfg.product_id == 0xbeef) {
        // check if we've already discovered this vehicle
        if ([self findVehicleById:mfg.identifier] != nil) {
            return;
        }
        NSLog(@"Discovered new vehicle, model=0x%04x, id=0x%08x", mfg.model_id, mfg.identifier);
        anki_vehicle_adv_info_t local_name;
        NSData *name_data = [peripheral.name dataUsingEncoding:NSASCIIStringEncoding];
        anki_vehicle_parse_local_name(name_data.bytes, peripheral.name.length, &local_name);
        
        Vehicle *v = [Vehicle new];
        v.peripheral = peripheral;
        v.identifier = mfg.identifier;
        v.modelIdentifier = mfg.model_id;
        [_vehicles addObject:v];
        
        [peripheral setDelegate:v];
        [central connectPeripheral:peripheral options:nil];
    }
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals
{
    NSLog(@"Retrieved peripheral: %lu - %@", [peripherals count], peripherals);
}

-(void) vehicleWasDisconnected:(Vehicle *)v
{
    [_vehicles removeObject:v];
}

@end
