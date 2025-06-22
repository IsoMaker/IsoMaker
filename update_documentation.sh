git pull origin main;
if [ $? -ne 0 ]; then
    echo "Failed to pull the latest changes from the dev branch."
    exit 1
fi
rm -rf docs;
if [ $? -ne 0 ]; then
    echo "Failed to remove the existing docs directory."
    exit 1
fi
mkdir docs;
if [ $? -ne 0 ]; then
    echo "Failed to create the docs directory."
    exit 1
fi
doxygen Doxyfile;
if [ $? -ne 0 ]; then
    echo "Doxygen documentation generation failed."
    exit 1
fi
mv docs/html/* docs/;
if [ $? -ne 0 ]; then
    echo "Failed to move generated documentation to docs directory."
    exit 1
fi
rm -rf docs/html;
if [ $? -ne 0 ]; then
    echo "Failed to remove the empty html directory."
    exit 1
fi
echo "Documentation updated successfully in the docs directory."