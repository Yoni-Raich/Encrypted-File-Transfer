 Server Specification

1. Server.py
תפקיד: המחלקה הראשית שמנהלת את כל פעולות השרת.

תכונות:
- clientHandler: אובייקט של מחלקת ClientHandler
- cryptoManager: אובייקט של מחלקת CryptoManager
- databaseManager: אובייקט של מחלקת DatabaseManager

פונקציות:
- main(): הפונקציה הראשית שמתחילה את כל התהליך
- start(): מתחילה את השרת ומקבלת חיבורים מלקוחות
- handleRequest(request): מטפלת בבקשות שמתקבלות מהלקוחות

2. ClientHandler.py
תפקיד: אחראית על טיפול בחיבורים מלקוחות.

פונקציות:
- acceptClient(): מקבלת חיבור חדש מלקוח
- readRequest(): קוראת בקשה מהלקוח
- sendResponse(response): שולחת תשובה ללקוח

3. CryptoManager.py
תפקיד: אחראית על כל פעולות ההצפנה והפענוח.

פונקציות:
- generateRSAKeys(): מייצרת זוג מפתחות RSA
- encryptAES(data, key): מצפינה מידע עם מפתח AES
- decryptAES(data, key): מפענחת מידע מוצפן עם מפתח AES
- encryptRSA(data, public_key): מצפינה מידע עם מפתח RSA ציבורי
- decryptRSA(data, private_key): מפענחת מידע מוצפן עם מפתח RSA פרטי

4. DatabaseManager.py
תפקיד: אחראית על ניהול מסד הנתונים SQLite.

פונקציות:
- connect(): מתחברת למסד הנתונים
- createTables(): יוצרת את הטבלאות הנדרשות
- insertClient(client_data): מוסיפה רשומת לקוח חדשה
- insertFile(file_data): מוסיפה רשומת קובץ חדשה
- queryClient(client_id): מחזירה נתוני לקוח
- queryFile(file_id): מחזירה נתוני קובץ

5. Protocol.py
תפקיד: מגדירה את פרוטוקול התקשורת.

פונקציות:
- getRequestCode(action): מחזירה את קוד הבקשה המתאים לפעולה
- getResponseCode(action): מחזירה את קוד התשובה המתאים לפעולה

מידע נוסף למפתח:
1. השתמש בספריות הסטנדרטיות של Python לביצוע פעולות הצפנה ופענוח.
2. ודא שאתה מטפל בשגיאות ומצבי קצה בכל הפונקציות.
3. השתמש ב-SQLite3 לניהול מסד הנתונים.
4. השתמש בלוגים מפורטים לצורך ניפוי באגים ומעקב אחר פעולות המערכת.
5. ודא שאתה מיישם את פרוטוקול התקשורת בדיוק כפי שהוגדר, כולל כל השדות וגדלי המידע.