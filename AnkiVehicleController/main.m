//
//  main.m
//  AnkiVehicleController
//
//  Created by Jeff on 4/24/15.
//  Copyright (c) 2015 Jeff. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VehiclesController.h"
#import "StdInReader.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSLog(@"Anki vehicle controller startup");
        VehiclesController *controller = [VehiclesController new];
        [controller beginScan];
    
        StdInReader *stdInReader = [[StdInReader alloc] initWithVehicleController:controller];
        [stdInReader begin];
    
        [[NSRunLoop currentRunLoop] run];
        
        
    }
    return 0;
}
