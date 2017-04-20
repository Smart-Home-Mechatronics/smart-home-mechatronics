var ss = SpreadsheetApp.openById('14IKLbH5p99_tNnJ5omUw2Yt6BOUb_9TCwxhgrqldsXo');
var sheet = ss.getSheetByName('Sheet1');

function doGet(e)
{
  var readNotificationStatus = e.parameter.readNotificationStatus;
  var writeNotificationStatus = e.parameter.writeNotificationStatus;
  
  var readMotionDetection = e.parameter.readMotionDetection;
  var writeMotionDetection = e.parameter.writeMotionDetection;
  
  var readSetTemperature = e.parameter.readSetTemperature;
  var writeSetTemperature = e.parameter.writeSetTemperature;
  
  var readActualTemperature = e.parameter.readActualTemperature;
  var writeActualTemperature = e.parameter.writeActualTemperature;

  var readThermostatMode = e.parameter.readThermostatMode;
  var writeThermostatMode = e.parameter.writeThermostatMode;
  
  var readPIR = e.parameter.readPIR;
  var writePIR = e.parameter.writePIR;

  var readImportantEmails = e.parameter.readImportantEmails;
  var readSignificantOtherEmails = e.parameter.readSignificantOtherEmails;
  var writeImportantEmail = e.parameter.writeImportantEmail;
  var writeSignificantOtherEmail = e.parameter.writeSignificantOtherEmail;
  var deleteImportantEmailatIndex = e.parameter.deleteImportantEmailatIndex;
  var deleteSignificantOtherEmailatIndex = e.parameter.deleteSignificantOtherEmailatIndex;
  
  var readSetActualModeTemperatureLastSeenPIR = e.parameter.readSetActualModeTemperatureLastSeenPIR;
  
  var writePIRActualreadSetTempModeOutput1Output2 = e.parameter.writePIRActualreadSetTempModeOutput1Output2;
  
  if (writePIRActualreadSetTempModeOutput1Output2 != undefined)
  {
    var incomingData = writePIRActualreadSetTempModeOutput1Output2.split(",");
    
    // actual temp
    sheet.getRange('F2').setValue(incomingData[1]); 

    // PIR
    if(incomingData[0] == "1")
    {
      var date = new Date();
      sheet.getRange(2,1,1,1).setValue(date);// set time stamp to cell A2
      sheet.getRange(2,2,1,1).setValue("1"); // set 1 to PIR
    }
    
    // send the setTemp, Mode, Output1, and Output2
    var SetTemp = sheet.getRange('E2').getValue();
    var Mode = sheet.getRange('G2').getValue();
    var Output1 = sheet.getRange('K2').getValue();
    var Output2 = sheet.getRange('L2').getValue();
    
    return ContentService.createTextOutput("SetTempModeOutput1Output2," + SetTemp + "," + Mode + "," + Output1 + "," + Output2 + ","  + sheet.getRange('C2').getValue());
  }
  else if (readNotificationStatus != undefined) 
  {
    return ContentService.createTextOutput("notification," + sheet.getRange('C2').getValue());
  }
  else if (readSetActualModeTemperatureLastSeenPIR != undefined) 
  {
    // calculate LastSeenPIR
    var dateNow = new Date();
    var datePrev = new Date(sheet.getRange(2,1,1,1).getValue());
    var diff = dateNow - datePrev;
    
    return ContentService.createTextOutput("SetActualModeLastSeenPIR," + sheet.getRange('E2').getValue() + "," + sheet.getRange('F2').getValue() + "," + sheet.getRange('G2').getValue() + "," + diff);
  }
  else if (writeSetTemperature != undefined) 
  {
    sheet.getRange('E2').setValue(writeSetTemperature);
    //return ContentService.createTextOutput("SetTemperature," + writeSetTemperature);
  }
  else if (readSetTemperature != undefined) 
  {
    var setTempValue = sheet.getRange('E2').getValue();
    return ContentService.createTextOutput("SetTemperature," + setTempValue);
  }
  else if (writeThermostatMode != undefined) 
  {
    sheet.getRange('G2').setValue(writeThermostatMode);
    //return ContentService.createTextOutput("ThermostatMode," + writeThermostatMode);
  }
  else if (readThermostatMode != undefined) 
  {
    var ThermostatMode = sheet.getRange('G2').getValue();
    return ContentService.createTextOutput("ThermostatMode," + ThermostatMode);
  }
  else if (writeMotionDetection != undefined)
  {
    sheet.getRange('D2').setValue(writeMotionDetection);
  }
  else if (readMotionDetection != undefined)
  {
    return ContentService.createTextOutput("MotionDetection," + sheet.getRange('D2').getValue());
  }
  else if (writeActualTemperature != undefined) 
  {
    sheet.getRange('F2').setValue(writeActualTemperature);
  }
  else if (readImportantEmails != undefined)
  {
    // find the last element in H
    var HValues = sheet.getRange("H1:H").getValues();
    var HLastElement = HValues.filter(String).length;
    var emailLists = "";
    for (var i = 2; i <= HLastElement; i++ )
    {
      emailLists += sheet.getRange(i,8,1,1).getValue()
      if(i != HLastElement)
      {
        emailLists += ",";
      }
    }
    return ContentService.createTextOutput("ImportantEmails," + emailLists);
  }
  else if (readSignificantOtherEmails != undefined)
  {
    // find the last element in I
    var IValues = sheet.getRange("I1:I").getValues();
    var ILastElement = IValues.filter(String).length;
    var emailLists = "";
    for (var i = 2; i <= ILastElement; i++ )
    {
      emailLists += sheet.getRange(i,9,1,1).getValue()
      if(i != ILastElement)
      {
        emailLists += ",";
      }
    }
    return ContentService.createTextOutput("SignificantOtherEmails," + emailLists);
  }
  else if (writeImportantEmail != undefined)
  {
    // find the last element in H
    var HValues = sheet.getRange("H1:H").getValues();
    var HLastElement = HValues.filter(String).length;
    sheet.getRange(HLastElement + 1,8,1,1).setValue(writeImportantEmail)
  }
  else if (writeSignificantOtherEmail != undefined)
  {
    // find the last element in I
    var IValues = sheet.getRange("I1:I").getValues();
    var ILastElement = IValues.filter(String).length;
    sheet.getRange(ILastElement + 1,9,1,1).setValue(writeSignificantOtherEmail)
  }
  else if (deleteImportantEmailatIndex != undefined)
  {
    // find the last element in H
    var HValues = sheet.getRange("H1:H").getValues();
    var HLastElement = HValues.filter(String).length;
    // delte the cell
    var index = Number(deleteImportantEmailatIndex) + 2;
    sheet.getRange(index,8,1,1).clear();
    
    // shift the next ones up 
    for(var i = index; i <= HLastElement; i++)
    {
      sheet.getRange(i,8,1,1).setValue(sheet.getRange(i+1,8,1,1).getValue());
    }
  }
  else if (deleteSignificantOtherEmailatIndex != undefined)
  {
    // find the last element in I
    var IValues = sheet.getRange("I1:I").getValues();
    var ILastElement = IValues.filter(String).length;
    // delte the cell
    var index = Number(deleteSignificantOtherEmailatIndex) + 2;
    sheet.getRange(index,9,1,1).clear();
    
    // shift the next ones up 
    for(var i = index; i <= ILastElement; i++)
    {
      sheet.getRange(i,9,1,1).setValue(sheet.getRange(i+1,9,1,1).getValue());
    }
  }
  else if (writePIR != undefined)
  {
    var date = new Date();
    sheet.getRange(2,1,1,1).setValue(date);// set time stamp to cell A2
    sheet.getRange(2,2,1,1).setValue("1"); // set 1 to PIR
  }
  else if (readPIR != undefined)
  {    
    var dateNow = new Date();
    var datePrev = new Date(sheet.getRange(2,1,1,1).getValue());
    var diff = dateNow - datePrev;
    return ContentService.createTextOutput("LastSeenPIR," + diff);
  }
  
}



function runEveryMinuteWithAutomaticTrigger()
{
  // 10 times check the email every 5 seconds
  for (var i = 0 ; i < 10; i++)
  {
    checkLastEmailSetProperNotification();
    Utilities.sleep(5000); 
  }
}

// Notification modes:
// 0: White
// 1: RomanticAlert
// 2: NormalAlert
// 3: EmergencyAlert
function checkLastEmailSetProperNotification()
{
  // get the 'From' of last email
  var message = GmailApp.getInboxThreads(0, 1)[0].getMessages()[0];
  
  // if it is unread go for the notification check.... otherwise make the lamp steady white
  if (message.isUnread())
  {
    var messageFrom = message.getFrom();
    Logger.log("Message from: " + messageFrom);
    
    // booleans to check if the email address is Important or Romantic
    var isImportant = false;
    var isRomantic = false;
    
    // get the list of important emails from the googlesheet
    // find the last element in H
    var HValues = sheet.getRange("H1:H").getValues();
    var HLastElement = HValues.filter(String).length;
    
    for (var i = 2; i <= HLastElement; i++ )
    {
      // if it contains the important email address 
      var thisImportantEmailAddress = sheet.getRange(i,8,1,1).getValue();
      if(messageFrom.indexOf(thisImportantEmailAddress)>-1)
      {
        isImportant = true;
        break;
      }
    }
    
    // get the list of Romantic emails from the googlesheet
    // find the last element in I
    var IValues = sheet.getRange("I1:I").getValues();
    var ILastElement = IValues.filter(String).length;
    
    if (isImportant == false)
    {
      for (var i = 2; i <= ILastElement; i++ )
      {
        // if it contains the important email address 
        var thisRomanticEmailAddress = sheet.getRange(i,9,1,1).getValue();
        if(messageFrom.indexOf(thisRomanticEmailAddress)>-1)
        {
          isRomantic = true;
          break;
        }
      }
    }

    // Notification modes:
    // 0: White
    // 1: Normal Alert
    // 2: RomanticAlert
    // 3: EmergencyAlert
    
    if(isImportant)
    {
      sheet.getRange('C2').setValue("3");
    }
    else if (isRomantic)
    {
      sheet.getRange('C2').setValue("2");
    }
    else
    {
      // steady white
      sheet.getRange('C2').setValue("0");
    } 
  }
  else
  {
    // steady white
    sheet.getRange('C2').setValue("0");
  }
}
//https://script.google.com/macros/s/AKfycbxI7o_wVNI_aY59fME9SLSCVcpwqY--vIeXG7G6hj2AXEba_YgF/exec?writePIR=1
//https://script.google.com/macros/s/AKfycbxI7o_wVNI_aY59fME9SLSCVcpwqY--vIeXG7G6hj2AXEba_YgF/exec?writePIRActualreadSetTempModeOutput1Output2=1,23