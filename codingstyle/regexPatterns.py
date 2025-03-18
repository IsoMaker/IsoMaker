classBase = r'class\s+'
classPattern = classBase + r'(\w+)'
className = r'[A-Z][a-z]+([A-Z][a-z]+)*'  # Matches UpperCamelCase class names
interfaceAbstractName = r'A|I' + className  # Matches IUpperCamelCase interface names
