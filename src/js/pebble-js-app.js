// Watchface config version
var version = 42;

function buildConfigUrl(hasColorScreen) {
  var html = "<!doctype html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>" +
    "<title>Fuzzy Text Plus Settings</title>" +
    "<style>body{font-family:Helvetica,Arial,sans-serif;padding:16px;background:#f6f7f8;color:#1f2328}h3{margin-top:0}" +
    "label{display:block;margin:10px 0 6px;font-size:14px}input,select{width:100%;padding:10px;border:1px solid #c8ccd1;border-radius:6px;box-sizing:border-box}" +
    ".row{margin-bottom:10px}.muted{font-size:12px;color:#5a6169}button{margin-top:16px;padding:10px 12px;border:0;border-radius:6px;background:#0b57d0;color:#fff;font-size:15px;width:100%}" +
    ".hidden{display:none}</style></head><body>" +
    "<h3>Fuzzy Text Plus Settings</h3>" +
    "<div class='row'><label>Language</label><select id='language'>" +
    "<option value='1'>Svenska</option><option value='2'>English</option><option value='3'>Norsk</option><option value='4'>Nederlands</option>" +
    "<option value='5'>Italiano</option><option value='6'>Español</option><option value='7'>Deutsch (O)</option><option value='8'>Deutsch (w)</option></select></div>" +
    "<div id='bwRow' class='row hidden'><label><input id='inverse' type='checkbox'> Inverse colors</label></div>" +
    "<div id='colorRows' class='hidden'>" +
    "<div class='row'><label>Background color (0xRRGGBB)</label><input id='background' type='text' value='0x000000'></div>" +
    "<div class='row'><label>Regular text color (0xRRGGBB)</label><input id='regular' type='text' value='0xFFFFFF'></div>" +
    "<div class='row'><label>Bold text color (0xRRGGBB)</label><input id='bold' type='text' value='0xFFFFFF'></div>" +
    "</div>" +
    "<div class='row'><label>Time offset, minutes</label><select id='offset'>" +
    "<option value='0'>0:00</option><option value='60'>1:00</option><option value='120'>2:00</option><option value='150'>2:30</option>" +
    "<option value='180'>3:00</option><option value='240'>4:00</option><option value='300'>5:00</option></select></div>" +
    "<div class='row'><label>Greeting display time</label><select id='message_time'>" +
    "<option value='0'>off</option><option value='2'>2 seconds</option><option value='3'>3 seconds</option><option value='4'>4 seconds</option></select></div>" +
    "<div class='row'><label>Show date on gesture</label><select id='gesture'>" +
    "<option value='0'>Off</option><option value='2'>Flick wrist</option><option value='3'>Shake up/down</option><option value='1'>Boxing move</option><option value='4'>Any shake</option></select></div>" +
    "<div class='row'><label>Connection lost</label><select id='bt'>" +
    "<option value='0'>Off</option><option value='1'>Message only</option><option value='2'>Buzz and message</option></select></div>" +
    "<button id='save'>Save</button><p class='muted'>Version: " + version + "</p>" +
    "<script>(function(){var hasColor=" + (hasColorScreen ? "true" : "false") + ";" +
    "var defaults={inverse_colors:false,background_color:'0x000000',regular_color:'0xFFFFFF',bold_color:'0xFFFFFF',language:'2',offset:'180',message_time:'3',gesture:'4',bt_notification:'2'};" +
    "function load(){try{var raw=localStorage.getItem('fuzzy_text_plus_settings');if(raw){return Object.assign({},defaults,JSON.parse(raw));}}catch(e){}return defaults;}" +
    "function save(v){try{localStorage.setItem('fuzzy_text_plus_settings',JSON.stringify(v));}catch(e){}}" +
    "var state=load();" +
    "if(hasColor){document.getElementById('colorRows').classList.remove('hidden');}else{document.getElementById('bwRow').classList.remove('hidden');}" +
    "document.getElementById('inverse').checked=!!state.inverse_colors;" +
    "document.getElementById('background').value=state.background_color||'0x000000';" +
    "document.getElementById('regular').value=state.regular_color||'0xFFFFFF';" +
    "document.getElementById('bold').value=state.bold_color||'0xFFFFFF';" +
    "document.getElementById('language').value=String(state.language||'2');" +
    "document.getElementById('offset').value=String(state.offset||'180');" +
    "document.getElementById('message_time').value=String(state.message_time||'3');" +
    "document.getElementById('gesture').value=String(state.gesture||'4');" +
    "document.getElementById('bt').value=String(state.bt_notification||'2');" +
    "document.getElementById('save').addEventListener('click',function(){var out={inverse_colors:document.getElementById('inverse').checked,background_color:document.getElementById('background').value,regular_color:document.getElementById('regular').value,bold_color:document.getElementById('bold').value,language:document.getElementById('language').value,offset:document.getElementById('offset').value,message_time:document.getElementById('message_time').value,gesture:document.getElementById('gesture').value,bt_notification:document.getElementById('bt').value};save(out);document.location='pebblejs://close#'+encodeURIComponent(JSON.stringify(out));});})();</script>" +
    "</body></html>";

  return 'data:text/html;charset=utf-8,' + encodeURIComponent(html);
}

function configDataToDict(configData) {
  var dict = {};
  dict.KEY_INVERSE = configData.inverse_colors ? 1 : 0;

  var argb = hexColorToARGB2222(configData.background_color);
  if (argb > 0) dict.KEY_BACKGROUND = argb;

  argb = hexColorToARGB2222(configData.regular_color);
  if (argb > 0) dict.KEY_REGULAR_TEXT = argb;

  argb = hexColorToARGB2222(configData.bold_color);
  if (argb > 0) dict.KEY_BOLD_TEXT = argb;

  var lang = configData.language;
  if (lang > 0) dict.KEY_LANGUAGE = parseInt(lang, 10);

  var offset = configData.offset;
  if (offset !== undefined) dict.KEY_OFFSET = parseInt(offset, 10);

  var messageTime = configData.message_time;
  if (messageTime !== undefined) dict.KEY_MESSAGE_TIME = parseInt(messageTime, 10);

  var gesture = configData.gesture;
  if (gesture !== undefined) dict.KEY_GESTURE = parseInt(gesture, 10);

  var btNotification = configData.bt_notification;
  if (btNotification !== undefined) dict.KEY_BT_NOTIFICATION = parseInt(btNotification, 10);

  return dict;
}

function hexColorToARGB2222(color) {
  if (color && color.length === 8) { // Expect "0xRRGGBB"
    var r = parseInt(color.substring(2, 3), 16) >> 2;
    var g = parseInt(color.substring(4, 5), 16) >> 2;
    var b = parseInt(color.substring(6, 7), 16) >> 2;

    var col = 3; // alpha
    col = (col << 2) + r;
    col = (col << 2) + g;
    col = (col << 2) + b;
    return col;
  }

  return 0;
}

function hasColorPlatform(platform) {
  return platform.indexOf('basalt') >= 0
    || platform.indexOf('chalk') >= 0
    || platform.indexOf('emery') >= 0
    || platform.indexOf('diorite') >= 0;
}

function hasColor() {
  if (Pebble.getActiveWatchInfo) {
    var platform = Pebble.getActiveWatchInfo().platform;
    return hasColorPlatform(platform);
  }

  return false;
}

if (typeof Pebble !== 'undefined') {
  Pebble.addEventListener('ready', function() {
    console.log('PebbleKit JS ready!');
  });

  Pebble.addEventListener('showConfiguration', function() {
    var url = buildConfigUrl(hasColor());
    console.log('Showing configuration page');
    Pebble.openURL(url);
  });

  Pebble.addEventListener('webviewclosed', function(e) {
    if (!e || !e.response) {
      return;
    }

    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));

    var dict = configDataToDict(configData);
    Pebble.sendAppMessage(dict, function() {
      console.log('Send successful: ' + JSON.stringify(dict));
    }, function(err) {
      console.log('Send failed! ' + JSON.stringify(err) + ', ' + JSON.stringify(dict));
    });
  });
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = {
    buildConfigUrl: buildConfigUrl,
    configDataToDict: configDataToDict,
    hasColorPlatform: hasColorPlatform,
    hexColorToARGB2222: hexColorToARGB2222
  };
}
