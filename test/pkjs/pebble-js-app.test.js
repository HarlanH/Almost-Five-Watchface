const test = require('node:test');
const assert = require('node:assert/strict');

const {
  buildConfigUrl,
  configDataToDict,
  hasColorPlatform,
  hexColorToARGB2222
} = require('../../src/js/pebble-js-app');

test('buildConfigUrl returns embedded data URL', () => {
  const url = buildConfigUrl(false);
  assert.match(url, /^data:text\/html;charset=utf-8,/);
  assert.match(decodeURIComponent(url), /pebblejs:\/\/close#/);
});

test('hasColorPlatform recognizes known color platforms', () => {
  assert.equal(hasColorPlatform('basalt'), true);
  assert.equal(hasColorPlatform('chalk'), true);
  assert.equal(hasColorPlatform('aplite'), false);
});

test('hexColorToARGB2222 converts valid color strings', () => {
  assert.equal(hexColorToARGB2222('0xFFFFFF'), 255);
  assert.equal(hexColorToARGB2222('0x000000'), 192);
  assert.equal(hexColorToARGB2222('#ffffff'), 0);
});

test('configDataToDict maps app settings keys', () => {
  const dict = configDataToDict({
    inverse_colors: true,
    background_color: '0x000000',
    regular_color: '0xFFFFFF',
    bold_color: '0xABCDEF',
    language: '2',
    offset: '180',
    message_time: '3',
    gesture: '4',
    bt_notification: '2'
  });

  assert.equal(dict.KEY_INVERSE, 1);
  assert.equal(dict.KEY_BACKGROUND, 192);
  assert.equal(dict.KEY_REGULAR_TEXT, 255);
  assert.equal(dict.KEY_BOLD_TEXT, 239);
  assert.equal(dict.KEY_LANGUAGE, 2);
  assert.equal(dict.KEY_OFFSET, 180);
  assert.equal(dict.KEY_MESSAGE_TIME, 3);
  assert.equal(dict.KEY_GESTURE, 4);
  assert.equal(dict.KEY_BT_NOTIFICATION, 2);
});
