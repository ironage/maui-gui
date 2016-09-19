
function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    // call default implementation first!
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/maui-gui.exe", "@StartMenuDir@/MAUI.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/res/hh",
            "iconId=2", "description=Open MAUI");
		component.addOperation("CreateShortcut", "@TargetDir@/maui-gui.exe", "@DesktopDir@/MAUI.lnk");
		component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/MaintenanceTool.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/res/hh",
            "iconId=3", "description=Update or Uninstall MAUI");
    }
}
