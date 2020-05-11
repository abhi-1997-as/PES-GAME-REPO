const serviceUuid = "19b10010-e8f2-537e-4f6c-d104768a1214";
const characteristicsUUID = {
  gesture_l:"19b10011-e8f2-537e-4f6c-d104768a1214",
  gesture_r:"19b10012-e8f2-537e-4f6c-d104768a1214",
  gesture_j:"19b10001-e8f2-537e-4f6c-d104768a1214",
  gesture_d:"19b10000-e8f2-537e-4f6c-d104768a1214"
}
let Gesture1Char;
let Gesture2Char;
let Gesture3Char;
let Gesture4Char;
let myBLE;

let left=0;
let right=0;
let jump=0;
let duck=0;

function setup() {
  createCanvas(400, 400);
    // Create a p5ble class
  myBLE = new p5ble();

  createCanvas(600, 400);
  background("#FFF");

  // Create a 'Connect and Start Notifications' button
  const connectButton = createButton('Connect and Start Notifications')
  connectButton.mousePressed(connectAndStartNotify);
  
}

function draw() {
   noStroke();

  if(left>0){
    fill(color(200, 200, 200));
  }else if(right>0){
    fill(color(100, 200, 200));
  }else if(jump>0){
    fill(color(200, 100, 200));
  }else if(duck>0){
    fill(color(200, 200, 100));
  }else{
    fill(color(100, 100, 100));
  }
  
  rect(15, 40, 60, 60);
}

function connectAndStartNotify() {
  // Connect to a device by passing the service UUID
  myBLE.connect(serviceUuid, gotCharacteristics);
}

// A function that will be called once got characteristics
function gotCharacteristics(error, characteristics) {
  if (error) console.log('error: ', error);
  console.log(characteristics[1].uuid);
  for(let i = 0; i < characteristics.length;i++){
    if(characteristics[i].uuid == characteristicsUUID.gesture_l){
      Gesture1Char = characteristics[i];
      myBLE.startNotifications(Gesture1Char, left_color);
    }else if(characteristics[i].uuid == characteristicsUUID.gesture_r){
      Gesture2Char = characteristics[i];
      myBLE.startNotifications(Gesture2Char, right_color);
    }else if(characteristics[i].uuid == characteristicsUUID.gesture_j){
      Gesture3Char = characteristics[i];
      myBLE.startNotifications(Gesture3Char, jump_color);
    }else if(characteristics[i].uuid == characteristicsUUID.gesture_d){
      Gesture4Char = characteristics[i];
      myBLE.startNotifications(Gesture4Char, duck_color);
    }else{
      console.log("nothing");
    }
  }
  // Start notifications on the first characteristic by passing the characteristic
  // And a callback function to handle notifications
  
}

// A function that will be called once got characteristics
function left_color(data) {
  console.log('left: ', data);
  left = Number(data);
  myBLE.read(Gesture1Char,left_color );
}
function right_color(data) {
  console.log('right: ', data);
  right = Number(data);
   myBLE.read(Gesture2Char,right_color );
}
function jump_color(data) {
  console.log('jump: ', data);
  jump = Number(data);
   myBLE.read(Gesture3Char,jump_color );
}
function duck_color(data) {
  console.log('duck: ', data);
  duck = Number(data);
   myBLE.read(Gesture4Char,duck_color );
}
