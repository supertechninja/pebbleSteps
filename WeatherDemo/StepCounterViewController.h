//
//  StepCounterViewController.h
//  WeatherDemo
//
//  Created by Joshua McWilliams on 11/14/13.
//  Copyright (c) 2013 Pebble Technology Corp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <PebbleKit/PebbleKit.h>
#import <CoreMotion/CoreMotion.h>

@interface StepCounterViewController : UIViewController
@property (weak, nonatomic) IBOutlet UILabel *totalStepsLabel;
@property (weak, nonatomic) IBOutlet UILabel *totalDistanceLabel;
@property (weak, nonatomic) IBOutlet UILabel *pebbleStatusLabel;
- (void)setTargetWatch:(PBWatch*)watch;
- (IBAction)refreshSteps:(id)sender;
@end
