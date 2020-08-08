//
//  HFHexGlyphTable.h
//  HexFiend_2
//
//  Copyright © 2020 ridiculous_fish. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HFBinaryGlyphTable : NSObject

- (instancetype)initWithFont:(HFFont *)font;

@property (readonly) CGFloat advancement;
@property (readonly) const CGGlyph *table;

@end
