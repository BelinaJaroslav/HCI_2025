#pragma once

class App {
public:
    App();
    bool Init();
    void Run();
    ~App();

private:
    // Lab 01
    void IdentifyObjectByLuminance();
    void FindRedObjectInImage();
    void FindRedObjectInVideo();
    void FindFaceInVideo();
};
