//
//  Xtrace.h
//  Xtrace
//
//  Created by John Holdsworth on 28/02/2014.
//  Copyright (c) 2014 John Holdsworth. All rights reserved.
//
//  Repo: https://github.com/johnno1962/Xtrace
//
//  $Id: //depot/Xtrace/Xray/Xtrace.h#20 $
//
//  Class to intercept messages sent to a class or object.
//  Swizzles generic logging implemntation in place of the
//  original which is called after logging the message.
//
//  Implemented as category  on NSObject so message the
//  class or instance you want to log for example:
//
//  Log all messages of the navigation controller class
//  and it's superclasses:
//  [UINavigationController xtrace]
//
//  Log all messages sent to objects instance1/2
//  [instance1 xtrace];
//  [instance2 xtrace];
//
//  Instance tracing takes priority.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifdef DEBUG
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

#ifdef __clang__
#if __has_feature(objc_arc)
#define XTRACE_ISARC
#endif
#endif

#ifdef XTRACE_ISARC
#define XTRACE_UNSAFE __unsafe_unretained
#define XTRACE_BRIDGE(_type) (__bridge _type)
#define XTRACE_RETAINED __attribute((ns_returns_retained))
#else
#define XTRACE_UNSAFE
#define XTRACE_BRIDGE(_type) (_type)
#define XTRACE_RETAINED
#endif

#define XTRACE_EXCLUSIONS \
    "^(allocWithZone:|initWithCoder:|_UIAppearance_|_hasBaseline|"\
    "_(initializeFor|performUpdatesForPossibleChangesOf)Idiom:|"\
    "timeIntervalSinceReferenceDate)|WithObjects(AndKeys)?:$"

@interface NSObject(Xtrace)

// dump class
+ (void)xdump;

// avoid a class
+ (void)notrace;

// trace class or..
+ (void)xtrace;

// trace instance
- (void)xtrace;

// stop tacing "
- (void)notrace;

@end

// implementing class
@interface Xtrace : NSObject

// delegate for callbacks
+ (void)setDelegate:delegate;

// hide log of return values
+ (void)hideReturns:(BOOL)hide;

// attempt log of call arguments
+ (void)showArguments:(BOOL)show;

// log values's "description"
+ (void)describeValues:(BOOL)desc;

// property methods filtered out by default
+ (void)includeProperties:(BOOL)include;

// include/exclude methods matching pattern
+ (BOOL)includeMethods:(NSString *)pattern;
+ (BOOL)excludeMethods:(NSString *)pattern;
+ (BOOL)excludeTypes:(NSString *)pattern;

// don't trace this class e.g. [UIView notrace]
+ (void)dontTrace:(Class)aClass;

// trace class down to NSObject
+ (void)traceClass:(Class)aClass;

// trace class down to "levels" of superclases
+ (void)traceClass:(Class)aClass levels:(int)levels;

// "kitchen sink" trace all classes matching pattern
+ (void)traceClassPattern:(NSString *)pattern excluding:(NSString *)exclusions;

// trace all messages sent to an instance
+ (void)traceInstance:(id)instance;

// stop tracing messages to instance
+ (void)notrace:(id)instance;

// dump runtime class info
+ (void)dumpClass:(Class)aClass;

// before, replacement and after callbacks
+ (void)forClass:(Class)aClass before:(SEL)sel callback:(SEL)callback;
+ (void)forClass:(Class)aClass replace:(SEL)sel callback:(SEL)callback;
+ (void)forClass:(Class)aClass after:(SEL)sel callback:(SEL)callback;

// internal information
#define XTRACE_ARGS_SUPPORTED 10

typedef void (*VIMP)( id obj, SEL sel, ... );

struct _xtrace_arg {
    const char *name, *type;
    int stackOffset;
};

// information about original implementations
struct _xtrace_info {
    int depth;
    Method method;
    XTRACE_UNSAFE id lastObj;
    VIMP before, original, after;
    const char *name, *type, *mtype;
    struct _xtrace_arg args[XTRACE_ARGS_SUPPORTED+1];

    struct _stats {
        NSTimeInterval entered, elapsed;
        unsigned callCount;
    } stats;
    BOOL logged, callingBack;
};

// includes argument info and recorded stats
+ (struct _xtrace_info *)infoFor:(Class)aClass sel:(SEL)sel;

@end
#endif
#endif
