#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <stdexcept>
#include <string>

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class GuiInterface {
 public:
  GLFWwindow* window;

  GuiInterface(const std::string& title) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup style
    ImGui::StyleColorsDark();

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw new std::runtime_error("GLFW init failed");

    // Create window with graphics context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, title.c_str(), NULL, NULL);
    if (window == NULL)
        throw new std::runtime_error("GLFW window not created");
  }


  virtual ~GuiInterface() {
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

  }

  void Run() {
    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    id <MTLCommandQueue> commandQueue = [device newCommandQueue];

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);

    NSWindow *nswin = glfwGetCocoaWindow(window);
    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswin.contentView.layer = layer;
    nswin.contentView.wantsLayer = YES;

    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor new];
    float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    while(!glfwWindowShouldClose(window)) {
      @autoreleasepool { 
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        layer.drawableSize = CGSizeMake(width, height);
        id<CAMetalDrawable> drawable = [layer nextDrawable];
        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];

        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
        renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder pushDebugGroup:@"dummy"];

        // Start the Dear ImGui frame
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Render();

        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];

        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
      }
    }
  }
  virtual void Render() = 0;
};
