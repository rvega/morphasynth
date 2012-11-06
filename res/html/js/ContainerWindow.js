/**
 * @class ContainerWindow_
 * 
 * Provides communication with the HTMLWindow object that contains the 
 * WebKit instance where this Javascript is running.
 *
 * This class and an instance are created automatically by QTWebKit but we create 
 * them here for use when running the html app in a regular web browser (easier for development).
 *
 * Avaliable methods are the ones created here and also the ones marked as "slots" in HTMLWindow.h
 *
 */
if(typeof(ContainerWindow) == 'undefined'){
   ContainerWindow_ = function(){
      this.log = function(msg){
         console.log(msg);
      }

      this.guiDidLoad = function(ok){
         console.log("Sending DidLoad event to ContainerWindow");
      }
   };

   ContainerWindow = new ContainerWindow_();
}

/**
 * This function is triggered when c++ sends an event to us. Users must override it.
 */
ContainerWindow.didReceiveEvent = function(evt){
   alert("You must implement the ContainerWindow.didReceiveEvent(evt) function in your javascript!");
}

/**
 * Tells c++ that we're ready to roll. 
 *
 * Users can implement ContainerWindow.ready() if they need to perform aditional initialization.
 */
$(document).ready(function(){
   if(typeof(ContainerWindow.ready) == 'function'){
      ContainerWindow.ready(); 
   }

   ContainerWindow.guiDidLoad(true);
})

