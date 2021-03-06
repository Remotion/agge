#import "ShellView.h"

@implementation ShellView
	- (void) createApp
	{
		_services.reset(new shell_services);
		_surface.reset(new platform_bitmap(1, 1));
		_application.reset(agge_create_application(*_services));
	}

	- (id) init
	{
		[self createApp];
		return [super init];
	}

	- (id) initWithFrame:(NSRect)frameRect
	{
		[self createApp];
		return [super initWithFrame:frameRect];
	}

	- (id) initWithCoder:(NSCoder *)decoder
	{
		[self createApp];
		return [super initWithCoder:decoder];
	}

	- (void) setFrameSize:(NSSize)newSize
	{
		_surface->resize(static_cast<unsigned>(newSize.width), static_cast<unsigned>(newSize.height));
		_application->resize(static_cast<unsigned>(newSize.width), static_cast<unsigned>(newSize.height));
		[super setFrameSize:newSize];
	}

	- (void) drawRect:(NSRect)dirtyRect
	{
		application::timings t;
		CGContextRef context = static_cast<CGContextRef>([[NSGraphicsContext currentContext]graphicsPort]);
		
		_application->draw(*_surface, t);
		_surface->blit(context, 0, 0, _surface->width(), _surface->height());
		
		CGAffineTransform  deviceTransform =
		CGContextGetUserSpaceToDeviceSpaceTransform(context);
		NSLog(@"x-scaling = %f y-scaling = %f", deviceTransform.a, deviceTransform.d);
	}
@end

application::~application()
{	}

void application::resize(int /*width*/, int /*height*/)
{
}

stream *shell_services::open_file(const char *path)
{
	throw 0;
}

double stopwatch(long long&)
{
	return 0;
}
