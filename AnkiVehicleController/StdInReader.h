//
//  StdInReader.h
//  AnkiVehicleController
//
//  Created by Jeff on 4/25/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VehiclesController.h"

@interface StdInReader : NSObject

@property (strong) VehiclesController* controller;

-(id) initWithVehicleController:(VehiclesController*)controller;
-(void) begin;

@end
