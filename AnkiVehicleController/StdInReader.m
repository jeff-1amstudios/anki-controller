//
//  StdInReader.m
//  AnkiVehicleController
//
//  Created by Jeff on 4/25/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import "StdInReader.h"
#import "Vehicle.h"

@implementation StdInReader

-(id) initWithVehicleController:(VehiclesController*)controller {
    self = [super init];
    _controller = controller;
    return self;
}


-(void) begin {
    [NSThread detachNewThreadSelector:@selector(listenForCommands) toTarget:self withObject:nil];
}

-(void) listenForCommands {
    
    /* { command: 'set-speed', carId: 'id', value: 30 }
       
     list-cars, 
     spin carId
     scan
     stop-scan
     
     */
    while (true) {
        char input[256];
        NSError *error = nil;
        fgets(input, 256, stdin);
        NSLog(@"Recieved command: %s\n", input);
        id object = [NSJSONSerialization
                     JSONObjectWithData: [NSData dataWithBytes:input length:strlen(input)]
                     options:0
                     error:&error];
        if (error) {
            NSLog(@"Invalid json: %@", [error localizedDescription]);
            continue;
        }
        NSDictionary *dict = object;
        NSString *command = dict[@"command"];
        if ([command isEqualToString:@"set-speed"]) {
            Vehicle *v = [self getVehicle:dict[@"carId"]];
            int val = (int)[dict[@"value"] integerValue];
            [v setSpeed:val];
        }
        else if ([command isEqualToString:@"list-cars"]) {
            NSMutableArray *cars = [NSMutableArray new];
            for (Vehicle *v in _controller.vehicles) {
                NSMutableDictionary *car = [NSMutableDictionary new];
                [car setObject:[NSNumber numberWithUnsignedInt:v.identifier] forKey:@"id"];
                [cars addObject:car];
            }
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:cars options:0 error:&error];
            NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            NSLog(@"**%@**\n", jsonString);
        }
        else if ([command isEqualToString:@"spin"]) {
            Vehicle *v = [self getVehicle:dict[@"carId"]];
            [v spin180];
        }
        else if ([command isEqualToString:@"scan"]) {
            [_controller beginScan];
        }
        else if ([command isEqualToString:@"stop-scan"]) {
            [_controller stopScan];
        }
        else {
            NSLog(@"Unrecognized command %@", command);
        }
    }
}

-(Vehicle*) getVehicle:(NSNumber*) carId {
    uint32_t identifier = (uint32_t)[carId integerValue];
    Vehicle *v = [_controller findVehicleById:identifier];
    if (v == nil) {
        NSLog(@"Failed to find vehicle id=%d", identifier);
    }
    return v;
}

@end
