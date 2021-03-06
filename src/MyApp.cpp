#include "MyApp.h"
#include <iostream>
#include "Models/PEA.h"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include <fstream>
#include "Utils/DataLoader/DataLoader.cpp"
using namespace std;
using namespace rapidjson;


MyApp::MyApp() {
    ///
    /// Create our main App instance.
    ///
    app_ = App::Create();

    /// Create a resizable window by passing by OR'ing our window flags with
    /// kWindowFlags_Resizable.
    ///
    window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
                             false, kWindowFlags_Titled | kWindowFlags_Resizable);

    ///
    /// Create our HTML overlay-- we don't care about its initial size and
    /// position because it'll be calculated when we call OnResize() below.
    ///
    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);

    ///
    /// Force a call to OnResize to perform size/layout of our overlay.
    ///
    OnResize(window_.get(), window_->width(), window_->height());

    ///
    /// Load a page into our overlay's View
    ///
    overlay_->view()->LoadURL("file:///app.html");

    ///
    /// Register our MyApp instance as an AppListener so we can handle the
    /// App's OnUpdate event below.
    ///
    app_->set_listener(this);

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the
    /// Window's OnResize event below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a LoadListener so we can handle the
    /// View's OnFinishLoading and OnDOMReady events below.
    ///
    overlay_->view()->set_load_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the
    /// View's OnChangeCursor and OnChangeTitle events below.
    ///
    overlay_->view()->set_view_listener(this);

}

MyApp::~MyApp() {
}

void MyApp::Run() {
    app_->Run();
}

void MyApp::OnUpdate() {
    ///
    /// This is called repeatedly from the application's update loop.
    ///
    /// You should update any app logic here.
    ///
}

void MyApp::OnClose(ultralight::Window *window) {
    app_->Quit();
}

void MyApp::OnResize(ultralight::Window *window, uint32_t width, uint32_t height) {
    ///
    /// This is called whenever the window changes size (values in pixels).
    ///
    /// We resize our overlay here to take up the entire window.
    ///
    overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View *caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String &url) {
    ///
    /// This is called when a frame finishes loading on the page.
    ///
}

JSValueRef openRegistryPage(JSContextRef ctx, JSObjectRef function,
                            JSObjectRef thisObject, size_t argumentCount,
                            const JSValueRef arguments[], JSValueRef *exception) {

    const char *str =
            "document.getElementById('registry_page').style.display = 'block';"
            "document.getElementById('add_registry_record').style.display = 'none';";

    JSStringRef script = JSStringCreateWithUTF8CString(str);

    JSEvaluateScript(ctx, script, 0, 0, 0, 0);

    JSStringRelease(script);

    return JSValueMakeNull(ctx);
}

JSValueRef updateRecords(JSContextRef ctx, JSObjectRef function,
                         JSObjectRef thisObject, size_t argumentCount,
                         const JSValueRef arguments[], JSValueRef *exception){

    DataLoader dataLoader;

    dataLoader.loadInProducts();

    std::list<PEA> peas = dataLoader.getPeas();
    std::list<CryptoCurrency> cryptoCurrencies = dataLoader.getCryptoCurrencies();
    std::list<Nft> nfts = dataLoader.getNfts();
    std::list<CompteDevise> compteDevises = dataLoader.getComptesDevises();

    string htmlPart = "<div class=\"row\"><div class=\"gauche\"><h2>Bancaire : </h2>";


    for(PEA pea : peas){
        string part ="<div class=\"card\">";
        part += "<div class=\"top\">" + pea.getName() + "</div>";
        part += "<div class=\"body\">";
        part += "<p class=\"name\"> Banque : <span class=\"value\">"+pea.getProviderName() + "</span> </p>";
        part += "<p class=\"name\">Actions :  <span class=\"value\">"+pea.getOrders() + "</span> </p>";
        part += "</div> </div>";

        htmlPart+=part;
    }

    for (CompteDevise cpt : compteDevises){
        string part ="<div class=\"card\">";
        part += "<div class=\"top\">" + cpt.getName() + "</div>";
        part += "<div class=\"body\">";
        part += "<p class=\"name\"> Banque : <span class=\"value\">"+cpt.getProviderName() + "</span> </p>";
        part += "<p class=\"name\"> Taux :  <span class=\"value\">"+std::to_string(cpt.getRate()) + "</span> </p>";
        part += "<p class=\"name\"> Solde :  <span class=\"value\">"+std::to_string(cpt.getAmount()) + "</span> </p>";
        part += "</div> </div>";

        htmlPart+=part;
    }
    htmlPart+="</div>";
    htmlPart+="<div class=\"droite\"><h2>Digital : </h2>";

    for (CryptoCurrency crypto : cryptoCurrencies){
        string part ="<div class=\"card\">";
        part += "<div class=\"top\">" + crypto.getName() + "</div>";
        part += "<div class=\"body\">";
        part += "<p class=\"name\"> Code monnaie : <span class=\"value\">"+ crypto.getToken() + "</span> </p>";
        part += "<p class=\"name\"> Solde :  <span class=\"value\">"+ std::to_string(crypto.getBalance()) + "</span> </p>";
        part += "</div> </div>";

        htmlPart+=part;
    }

    for(Nft nft : nfts){
        string part ="<div class=\"card\">";
        part += "<div class=\"top\">" + nft.getName() + "</div>";
        part += "<div class=\"body\">";
        part += "<p class=\"name\"> Nom : <span class=\"value\">"+ nft.getName() + "</span> </p>";
        part += "<p class=\"name\"> Seller :  <span class=\"value\">"+nft.getProviderName() + "</span> </p>";
        part += "<p class=\"name\"> Cote :  <span class=\"value\">"+ nft.getCurrentCote()  + "</span> </p>";
        part += "</div> </div>";

        htmlPart+=part;
    }

    htmlPart+= "</div></div>";
    const char *formatted = htmlPart.data();
    JSString str(formatted);

    return JSValueMakeString(ctx,str);
}


JSValueRef openRecordPage(JSContextRef ctx, JSObjectRef function,
                          JSObjectRef thisObject, size_t argumentCount,
                          const JSValueRef arguments[], JSValueRef *exception) {

    const char *str =
            "document.getElementById('registry_page').style.display = 'none';"
            "document.getElementById('add_registry_record').style.display = 'block';";

    JSStringRef script = JSStringCreateWithUTF8CString(str);

    JSEvaluateScript(ctx, script, 0, 0, 0, 0);

    JSStringRelease(script);

    return JSValueMakeNull(ctx);
}

JSValueRef searchByRecord(JSContextRef ctx, JSObjectRef function,
                          JSObjectRef thisObject, size_t argumentCount,
                          const JSValueRef arguments[], JSValueRef *exception) {

    JSStringRef ref = JSValueToStringCopy(ctx, arguments[0], 0) ;


    return JSValueMakeNull(ctx);
}

void MyApp::OnDOMReady(ultralight::View *caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String &url) {
    // Acquire the JS execution context for the current page.
    //
    // This call will lock the execution context for the current
    // thread as long as the Ref<> is alive.
    //
    RefPtr<JSContext> context = caller->LockJSContext();


    // Get the underlying JSContextRef for use with the
    // JavaScriptCore C API.
    JSContextRef ctx = context->ctx();
    SetListener(ctx, "openRegistryPage", openRegistryPage);
    SetListener(ctx, "openRecordPage", openRecordPage);
    SetListener(ctx, "searchByRecord", searchByRecord);
    SetListener(ctx,"updateRecords",updateRecords);

    //this->updateRecords();
}

void MyApp::SetListener(JSContextRef ctx, char *funName, JSValueRef function(JSContextRef ctx, JSObjectRef function,
                                                                             JSObjectRef thisObject,
                                                                             size_t argumentCount,
                                                                             const JSValueRef arguments[],
                                                                             JSValueRef *exception)) {

    // Create a JavaScript String containing the name of our callback.
    JSStringRef registryFunctionRef = JSStringCreateWithUTF8CString(funName);

    // Create a garbage-collected JavaScript function that is bound to our
    // native C callback 'OnButtonClick()'.
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, registryFunctionRef,
                                                        function);

    // Get the global JavaScript object
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // accessible from the page as 'OnButtonClick()'.
    JSObjectSetProperty(ctx, globalObj, registryFunctionRef, func, 0, 0);

    // Release the JavaScript String we created earlier.
    JSStringRelease(registryFunctionRef);

}

void MyApp::OnChangeCursor(ultralight::View *caller,
                           Cursor cursor) {
    ///
    /// This is called whenever the page requests to change the cursor.
    ///
    /// We update the main window's cursor here.
    ///
    window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View *caller,
                          const String &title) {
    ///
    /// This is called whenever the page requests to change the title.
    ///
    /// We update the main window's title here.
    ///
    window_->SetTitle(title.utf8().data());
}
