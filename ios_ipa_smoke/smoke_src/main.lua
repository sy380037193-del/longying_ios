local fileUtils = cc.FileUtils:getInstance()
local smokeJson = fileUtils:getStringFromFile("smoke.json")

print("[longying-ios-smoke] boot ok")
print("[longying-ios-smoke] smoke.json bytes: " .. tostring(#smokeJson))

local director = cc.Director:getInstance()
local scene = cc.Scene:create()
local visibleSize = director:getVisibleSize()
local origin = director:getVisibleOrigin()

local label = cc.Label:createWithSystemFont("Longying iOS Smoke", "Arial", 32)
label:setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2)
scene:addChild(label)

if director:getRunningScene() then
    director:replaceScene(scene)
else
    director:runWithScene(scene)
end
