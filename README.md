1. **Training Mode**:
   - In this mode, the user (student) performs the following actions:
     - Selects the "Start Training" menu item.
     - Enters their surname in a dialog window.
     - Applies some (one/several/all) UI controls of the application in a specific order: clicks buttons, enters text in tables and input fields, selects menu items, etc.
     - Selects the "Finish Training" menu item.
   The application:
   - Records the sequence of user (student) actions with timestamps.
   - Upon the user selecting the "Finish Training" menu item, saves all user actions to an XML file. The XML file format is developed by the task performer. The XML file structure description should be provided in the practice report.

2. **Checking Mode**:
   - In this mode, the user (teacher):
     - Selects the "Check Training Results" menu item.
     - Selects the surname of one of the students who have completed the training from the list in a dialog window.
   The application:
   - Reproduces for the teacher the sequence of actions of the selected student with a specified acceleration/deceleration.
