RACK_PLUGINS_FOLDER=`cat ".rack-plugins-folder"`

echo "### Deploying Modules to VCV Rack ###\n"
echo "Using folder $RACK_PLUGINS_FOLDER"

echo "### Building Modules ###\n"
make dist

echo "### Removing old version from Rack plugins ###"
rm -R "$RACK_PLUGINS_FOLDER/Sunfire Instruments"

echo "### Adding latest build to Rack plugins ###"
mv "./dist/Mindreau" "$RACK_PLUGINS_FOLDER"
