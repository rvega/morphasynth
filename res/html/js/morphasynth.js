Morphasynth = {}

// if(typeof Morphasynth.ContainerApp == "undefined"){
//    /*
//     * Mock class, simulates the C++ app that contains the webkit instance where this script runs
//     * If ran within the C++ app, this object will be provided to us
//     */ 
//    Morphasynth.ContainerApp = function(){
//       $('html').hide();
//       this.getPoints = function(){
//          var points = new Array();
//          for(var i = 0; i<100; i++) {
//             var point = {x: Math.random(), y:Math.random()}
//             points.push(point);
//          }
//          return points;
//       };
// 
//       this.print = function(message){
//          console.log(message);
//       }
// 
//       this.htmlDidLoad = function(){
//          main();
//       }
//    }
// 
//    ContainerApp = new Morphasynth.ContainerApp();
// }

/*
 * Tell the container app that we're ready
 */
$(document).ready(function(){
   ContainerApp.htmlDidLoad();
});

/*
 * Manages the options
 */
Morphasynth.Options = function(){
   this.x = 0;
   this.y = 1;

   this.init = function(){
      $('#select-x').val(0);
      $('#select-y').val(1);
      $('#select-x, #select-y').bind('change', $.proxy(this.changedOption, this));
   }

   this.changedOption = function(event){
      var select = $(event.target);
      if(select.attr('id')=='select-x'){
         this.x = select.val();
      }
      else{
         this.y = select.val();
      }
   }
}


/*
 * Manages the piano keyboard
 */
Morphasynth.Keyboard = function(){
   this.init = function(){
      $('.key').bind('click', $.proxy(this.clickedKey, this))
      $('#piano-scroll').scrollLeft( $("#piano").width()/2.56 );
   };

   this.clickedKey = function(event){
      var key = $(event.target);
      var octaveNumber = key.closest('.octave').attr('data-octave');
      var noteNumber = parseInt(octaveNumber*12) + parseInt(key.attr('data-note-number'));
      console.log(noteNumber);
   };
}

/*
 * Manages the 2D space
 */
Morphasynth.TimbreSpace = function(){
   this.canvas = null;
   this.height = 0;
   this.width = 0;

   this.init = function(){
      self = this;

      // Bind mouse events
      $('#timbre-space').bind('doubleclick', function(event){
         if(event.which == 3){ // Right click
            self.rightDoubleClick.call(self);
         }
         else{
            self.leftDoubleClick.call(self);
         }
      });
      
      $('#timbre-space').bind('click', function(event){
         if(event.which == 3){
            self.rightClick.call(self);
         }
         else{
            self.leftClick.call(self);
         }
      });

   
      // Create drawing surface
      var ts = $('#timbre-space');
      this.width = ts.width();
      this.height = ts.height();
      this.canvas = Raphael('timbre-space', ts.width(), ts.height());
      console.log(this.canvas);

      // Draw the points
      this.drawPoints();
   };

   this.drawPoints = function(points){
      var p, x, y;
      var points = ContainerApp.getPoints();  
      for(var i=0; i<points.length; i++) {
         p = points[i];
         x = p.x * this.width;
         y = p.y * this.height;
         this.canvas.circle(x, y, 5);
      }
   };

   this.leftDoubleClick = function(event){
      console.log('a');
   }

   this.leftClick = function(event){
      console.log('b');
   }
   
   this.rightDoubleClick = function(event){
      console.log('c');
   }

   this.rightClick = function(event){
      console.log('d');
   }
}


/*
 * Main
 */
function main(){
   ContainerApp.print("hello from js");

   Keyboard = new Morphasynth.Keyboard();
   Keyboard.init();

   Space = new Morphasynth.TimbreSpace();
   Space.init();

   Options = new Morphasynth.Options();
   Options.init();
}
