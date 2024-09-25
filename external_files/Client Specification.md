Client Specification

1. Client.cpp
תפקיד: המחלקה הראשית שמנהלת את כל פעולות הלקוח.

תכונות:
- fileHandler: אובייקט של מחלקת FileHandler
- cryptoManager: אובייקט של מחלקת CryptoManager
- networkManager: אובייקט של מחלקת NetworkManager

פונקציות:
- main(): הפונקציה הראשית שמתחילה את כל התהליך
- register(): מבצעת רישום של הלקוח בשרת
- connect(): מבצעת התחברות חוזרת לשרת
- sendFile(filename): שולחת קובץ מוצפן לשרת

2. FileHandler.cpp / FileHandler.h
תפקיד: אחראית על קריאה וכתיבה של קבצים.

פונקציות:
- readTransferInfo(): קוראת את תוכן הקובץ transfer.info
- readMeInfo(): קוראת את תוכן הקובץ me.info
- writeMeInfo(data): כותבת נתונים לקובץ me.info

3. CryptoManager.cpp / CryptoManager.h
תפקיד: אחראית על כל פעולות ההצפנה והפענוח.

פונקציות:
- generateRSAKeys(): מייצרת זוג מפתחות RSA
- encryptAES(data, key): מצפינה מידע עם מפתח AES
- decryptAES(data, key): מפענחת מידע מוצפן עם מפתח AES
- encryptRSA(data, public_key): מצפינה מידע עם מפתח RSA ציבורי
- decryptRSA(data, private_key): מפענחת מידע מוצפן עם מפתח RSA פרטי

4. NetworkManager.cpp / NetworkManager.h
תפקיד: אחראית על התקשורת עם השרת.

פונקציות:
- sendRequest(request): שולחת בקשה לשרת
- receiveResponse(): מקבלת תשובה מהשרת

5. Protocol.cpp / Protocol.h
תפקיד: מגדירה את פרוטוקול התקשורת.

פונקציות:
- getRequestCode(action): מחזירה את קוד הבקשה המתאים לפעולה
- getResponseCode(action): מחזירה את קוד התשובה המתאים לפעולה

מידע נוסף למפתח:
1. השתמש בספריית CryptoPP לביצוע פעולות הצפנה ופענוח.
2. ודא שאתה מטפל בשגיאות ומצבי קצה בכל הפונקציות.
3. השתמש ב-Winsock או Boost לתקשורת רשת.
4. הקפד על טיפול נכון בזיכרון כדי למנוע דליפות זיכרון.
5. השתמש בלוגים מפורטים לצורך ניפוי באגים ומעקב אחר פעולות המערכת.
6. ודא שאתה מיישם את פרוטוקול התקשורת בדיוק כפי שהוגדר, כולל כל השדות וגדלי המידע.