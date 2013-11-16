//
//  StepCounterViewController.m
//  WeatherDemo
//
//  Created by Joshua McWilliams on 11/14/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import "StepCounterViewController.h"

@interface StepCounterViewController () <PBPebbleCentralDelegate> {
    BOOL _watchIsConnected;
    __block NSInteger *numberOfStepsTotal;
    __block NSString *numberOfStepsString;
    __block int numberOfStepsInt;
    __block NSString *totalDistance;
}
@property (nonatomic, strong) CMStepCounter *stepCounter;
@property (nonatomic, strong) PBPebbleCentral *pebbleManager;
@property (nonatomic, strong) PBWatch *watch;
@end

@implementation StepCounterViewController


@synthesize totalStepsLabel, pebbleStatusLabel, totalDistanceLabel;



- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    
    [super viewDidLoad];
    
       _watchIsConnected = NO;
    
    [self refresh];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (PBPebbleCentral *)pebbleManager {
    
    if (!_pebbleManager) {
        _pebbleManager = [PBPebbleCentral defaultCentral];
        _pebbleManager.delegate = self;
    }
    return _pebbleManager;
    
}

- (void)refresh {
    
    // Initialize Pebble watch if it's not connected
    if (!_watchIsConnected) {
        
        [self setTargetWatch:[self.pebbleManager lastConnectedWatch]];
        [self logIfNoWatch];
        
    }
    
    self.stepCounter = [[CMStepCounter alloc] init];
    
}


- (void)logIfNoWatch {
    if (self.watch == nil) {
        pebbleStatusLabel.text = @"Pebble Not Connected";
        [pebbleStatusLabel setBackgroundColor:[UIColor redColor]];
    } else{
        pebbleStatusLabel.text = @"Pebble Connected";
        [pebbleStatusLabel setBackgroundColor:[UIColor greenColor]];
    }
}

- (void)setTargetWatch:(PBWatch*)watch {
    self.watch = watch;
    
    // NOTE:
    // For demonstration purposes, we start communicating with the watch immediately upon connection,
    // because we are calling -appMessagesGetIsSupported: here, which implicitely opens the communication session.
    // Real world apps should communicate only if the user is actively using the app, because there
    // is one communication session that is shared between all 3rd party iOS apps.
    
    // Test if the Pebble's firmware supports AppMessages / Weather:
    [watch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {
            // Configure our communications channel to target the weather app:
            // See demos/feature_app_messages/weather.c in the native watch app SDK for the same definition on the watch's end:
            uint8_t bytes[] = {0x42, 0xc8, 0x6e, 0xa4, 0x1c, 0x3e, 0x4a, 0x07, 0xb8, 0x89, 0x2c, 0xcc, 0xca, 0x91, 0x41, 0x98};
            NSData *uuid = [NSData dataWithBytes:bytes length:sizeof(bytes)];
            
            [watch appMessagesSetUUID:uuid];
            
            [watch appMessagesAddReceiveUpdateHandler:^BOOL(PBWatch *watch, NSDictionary *update) {
                NSLog(@"got update from pebble %@", update);
                [self refreshSteps];
                return YES;
            }];
            
            NSString *message = [NSString stringWithFormat:@"Yay! %@ supports AppMessages :D", [watch name]];
            [[[UIAlertView alloc] initWithTitle:@"Connected!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
            
            if (_watch != nil) {
                NSLog(@"Pebble connected!\n");
            }
            
        } else {
            
            NSString *message = [NSString stringWithFormat:@"Blegh... %@ does NOT support AppMessages :'(", [watch name]];
            [[[UIAlertView alloc] initWithTitle:@"Connected..." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        }
    }];
}


- (IBAction)refreshSteps:(id)sender {
    [self logIfNoWatch];
    
    @try {
        
        NSDate *now = [NSDate date];
        NSCalendar *gregorian = [[NSCalendar alloc]
                                 initWithCalendarIdentifier:NSGregorianCalendar];
        NSDateComponents *comps = [gregorian components:NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay | NSCalendarUnitHour fromDate:now];
        [comps setHour:0];
        NSDate *today = [gregorian dateFromComponents:comps];
        
        [self.stepCounter queryStepCountStartingFrom:today
                                                  to:now
                                             toQueue:[NSOperationQueue mainQueue]
                                         withHandler:^(NSInteger numberOfSteps, NSError *error) {
                                             
                                             // NSLog(@"%s %ld %@", __PRETTY_FUNCTION__, numberOfSteps, error);
                                             //weakSelf.title = [@(numberOfSteps) stringValue];
                                             numberOfStepsTotal = &numberOfSteps;
                                             numberOfStepsString = [@(numberOfSteps) stringValue];
                                             numberOfStepsInt = numberOfSteps;
                                             totalStepsLabel.text = [@(numberOfSteps) stringValue];
                                             NSLog(@"%@", numberOfStepsString);
                                             NSLog(@"%d", numberOfStepsInt);
                                             
                                    }];
        
        
        return;
    }
    @catch (NSException *exception) {
    }
}

-(void) refreshSteps{
    
    __block NSString *message = @"";
    
    NSDate *now = [NSDate date];
    NSCalendar *gregorian = [[NSCalendar alloc]
                             initWithCalendarIdentifier:NSGregorianCalendar];
    NSDateComponents *comps = [gregorian components:NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay | NSCalendarUnitHour fromDate:now];
    [comps setHour:0];
    NSDate *today = [gregorian dateFromComponents:comps];
    
    [self.stepCounter queryStepCountStartingFrom:today
                                              to:now
                                         toQueue:[NSOperationQueue mainQueue]
                                     withHandler:^(NSInteger numberOfSteps, NSError *error) {
                                         
                                         // NSLog(@"%s %ld %@", __PRETTY_FUNCTION__, numberOfSteps, error);
                                         //weakSelf.title = [@(numberOfSteps) stringValue];
                                         numberOfStepsTotal = &numberOfSteps;
                                         numberOfStepsString = [@(numberOfSteps) stringValue];
                                         numberOfStepsInt = numberOfSteps;
                                         totalStepsLabel.text = [@(numberOfSteps) stringValue];
                                         
                                         float distance;
                                         distance = (float)numberOfStepsInt / 2095.00;
                                         NSLog(@"%.2f", distance);
                                         totalDistanceLabel.text = [NSString stringWithFormat:@"%.2f mi", distance];
                                         
                                         NSLog(@"%d", numberOfStepsInt);
                                         
                                         NSNumber *temperatureKey = @(0); // This is our custom-defined key for the temperature string.
                                         NSNumber *distanceKey = @(1);
                                         NSDictionary *update = @{temperatureKey:[NSString stringWithFormat:@"%d", numberOfStepsInt], distanceKey:[NSString stringWithFormat:@"%.2f", distance]};
                                         //NSDictionary *update = @{temperatureKey:numberOfStepsInt };
                                         [self.watch appMessagesPushUpdate:update onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
                                             message = error ? [error localizedDescription] : @"Update sent!";
                                         }];
                                     }];
    

}

#pragma mark - PBPebbleCentralDelegate

// A Pebble watch has connected to the phone
- (void)pebbleCentral:(PBPebbleCentral *)central watchDidConnect:(PBWatch *)watch isNew:(BOOL)isNew {
    
    NSLog(@"Watch Connected: %@", [watch name]);
    [self setTargetWatch:watch];
    
}

// A Pebble watch has disconnected from the phone
- (void)pebbleCentral:(PBPebbleCentral *)central watchDidDisconnect:(PBWatch *)watch {
    
    NSLog(@"Watch Disconnected: %@", [watch name]);
    if ([watch isEqual:self.watch]) {
        [self setTargetWatch:nil];
    }
    
}

@end
