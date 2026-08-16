wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"

wrk.body = [[
{
    "batchName": "benchmark-batch",
    "items": [
        { "id": 1,  "name": "Item 1",  "price": 10.99,  "quantity": 1 },
        { "id": 2,  "name": "Item 2",  "price": 20.99,  "quantity": 2 },
        { "id": 3,  "name": "Item 3",  "price": 30.99,  "quantity": 3 },
        { "id": 4,  "name": "Item 4",  "price": 40.99,  "quantity": 4 },
        { "id": 5,  "name": "Item 5",  "price": 50.99,  "quantity": 5 },
        { "id": 6,  "name": "Item 6",  "price": 60.99,  "quantity": 6 },
        { "id": 7,  "name": "Item 7",  "price": 70.99,  "quantity": 7 },
        { "id": 8,  "name": "Item 8",  "price": 80.99,  "quantity": 8 },
        { "id": 9,  "name": "Item 9",  "price": 90.99,  "quantity": 9 },
        { "id": 10, "name": "Item 10", "price": 100.99, "quantity": 10 },
        { "id": 11, "name": "Item 11", "price": 110.99, "quantity": 11 },
        { "id": 12, "name": "Item 12", "price": 120.99, "quantity": 12 },
        { "id": 13, "name": "Item 13", "price": 130.99, "quantity": 13 },
        { "id": 14, "name": "Item 14", "price": 140.99, "quantity": 14 },
        { "id": 15, "name": "Item 15", "price": 150.99, "quantity": 15 },
        { "id": 16, "name": "Item 16", "price": 160.99, "quantity": 16 }
    ]
}
]]