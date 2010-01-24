//
//  PropertyReflector.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/15/10.
//  For license see LICENSE.TXT
//

#import "PropertyReflector.h"

@implementation PropertyReflector

- (id)initWithTableView:(NSTableView *)aTableView
{
	self = [super init];
	if (self)
	{
		tableView = aTableView;
		[tableView retain];
		
		[tableView setDataSource:self];
		[tableView setDelegate:self];
		
		cachedProperties = nil;
		reflectedObject = nil;
	}
	return self;
}

- (id)reflectedObject
{
	return reflectedObject;
}

- (void)setReflectedObject:(id)anObject
{
	if (cachedProperties && reflectedObject)
	{
		for (CachedProperty *property in cachedProperties)
			[reflectedObject removeObserver:self forKeyPath:[property name]];
	}
	
	[reflectedObject release];
	reflectedObject = anObject;
	[reflectedObject retain];
	
	Class reflectedClass = [reflectedObject class];
	uint propertyCount = 0U;
	objc_property_t *properties = class_copyPropertyList(reflectedClass, &propertyCount);
	
	[cachedProperties release];
	cachedProperties = [[NSMutableArray alloc] init];
	for (uint i = 0; i < propertyCount; i++)
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
				CachedProperty *cachedProperty = [[CachedProperty alloc] initWithName:propertyName
																		   attributes:attributes];
				[cachedProperties addObject:cachedProperty];
				
				// with Key-Value-Observing is simple to add automatic updates of table view
				[reflectedObject addObserver:self
								  forKeyPath:propertyName
									 options:NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew
									 context:NULL];
			}
		}
	}
	
	[cachedProperties sortUsingSelector:@selector(compare:)];
	
	free(properties);
	
	[tableView reloadData];
}

- (void)dealloc
{
	for (CachedProperty *property in cachedProperties)
		[reflectedObject removeObserver:self forKeyPath:[property name]];
	
	[reflectedObject release];
	[cachedProperties release];
	[tableView release];
	[super dealloc];
}

#pragma mark Property

- (CachedProperty *)propertyAtIndex:(int)index
{
	return (CachedProperty *)[cachedProperties objectAtIndex:index];
}

- (id)propertyValueAtIndex:(int)index
{
	CachedProperty *property = [self propertyAtIndex:index];
	id value = [reflectedObject valueForKey:[property name]];
	//NSLog(@"property = %@, value = %@", [property name], value);
	return value;
}

- (void)setPropertyValue:(id)value atIndex:(int)index
{
	CachedProperty *property = [self propertyAtIndex:index];
	[reflectedObject setValue:value forKey:[property name]];
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

	CachedProperty *property = [self propertyAtIndex:rowIndex];
	return [property name];
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
	return [cachedProperties count];
}

#pragma mark TableViewDelegate

- (NSCell *)tableView:(NSTableView *)aTableView 
dataCellForTableColumn:(NSTableColumn *)aTableColumn 
				  row:(NSInteger)rowIndex
{
	if (!aTableColumn)
		return nil;

	if ([[aTableColumn identifier] isEqualTo:@"value"])
	{
		CachedProperty *property = [self propertyAtIndex:rowIndex];
		return [property cell];
	}
	return nil;
}


@end
