//
//  PropertyReflector.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/15/10.
//  For license see LICENSE.TXT
//

#import "PropertyReflector.h"

@implementation PropertyReflector

- (id)initWithObject:(id)anObject tableView:(NSTableView *)aTableView
{
	self = [super init];
	if (self)
	{
		reflectedObject = anObject;
		[reflectedObject retain];
		
		tableView = aTableView;
		[tableView retain];
		
		Class reflectedClass = [reflectedObject class];
		uint propertyCount = 0U;
		objc_property_t *properties = class_copyPropertyList(reflectedClass, &propertyCount);
		
		cachedPropertyNames = [[NSMutableArray alloc] init];
		for (int i = propertyCount - 1; i >= 0; i--)
		{
			NSString *attributes = [NSString stringWithCString:property_getAttributes(properties[i]) 
													  encoding:NSASCIIStringEncoding];
			
			NSString *propertyName = [NSString stringWithCString:property_getName(properties[i]) 
														encoding:NSASCIIStringEncoding];			
			
			NSLog(@"%@:%@", propertyName, attributes);
			
			// no references, structs or pointers
			if (![attributes hasPrefix:@"T@"] && 
				![attributes hasPrefix:@"T{"] &&
				![attributes hasPrefix:@"T^"])
			{
				NSArray *components = [attributes componentsSeparatedByString:@","];
				NSLog(@"components: %@", components);
				// ignore readonly
				if ([components count] < 2 || ![[components objectAtIndex:1] isEqualTo:@"R"])
				{
					[cachedPropertyNames addObject:propertyName];
					
					// with Key-Value-Observing is simple to add automatic updates of table view
					[reflectedObject addObserver:self
									  forKeyPath:propertyName
										 options:NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew
										 context:NULL];
				}
			}
		}
		
		[tableView setDataSource:self];
		
		free(properties);
	}
	return self;
}

- (void)dealloc
{
	for (NSString *propertyName in cachedPropertyNames)
		[reflectedObject removeObserver:self forKeyPath:propertyName];
	
	[reflectedObject release];
	[cachedPropertyNames release];
	[tableView release];
	[super dealloc];
}

#pragma mark Property

- (NSString *)propertyNameAtIndex:(int)index
{
	return (NSString *)[cachedPropertyNames objectAtIndex:index];
}

- (id)propertyValueAtIndex:(int)index
{
	return [reflectedObject valueForKey:[self propertyNameAtIndex:index]];
}

- (void)setPropertyValue:(id)value atIndex:(int)index
{
	[reflectedObject setValue:value forKey:[self propertyNameAtIndex:index]];
}

- (void)observeValueForKeyPath:(NSString *)keyPath 
					  ofObject:(id)object
						change:(NSDictionary *)change
					   context:(void *)context
{
	[tableView reloadData];
}

#pragma mark TableViewDataSource

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn 
			row:(NSInteger)rowIndex
{
	if ([[aTableColumn identifier] isEqualTo:@"value"])
		return [self propertyValueAtIndex:rowIndex];
	return [self propertyNameAtIndex:rowIndex];
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject
   forTableColumn:(NSTableColumn *)aTableColumn
			  row:(NSInteger)rowIndex
{
	if ([[aTableColumn identifier] isEqualTo:@"value"])
	{
		[self setPropertyValue:anObject atIndex:rowIndex];
	}
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [cachedPropertyNames count];
}


@end
