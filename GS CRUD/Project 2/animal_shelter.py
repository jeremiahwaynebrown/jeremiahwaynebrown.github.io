from pymongo import MongoClient
from bson.objectid import ObjectId
import os

class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self):
        DB = 'AAC'
        COL = 'animals'

        try:
            self.client = MongoClient("mongodb://localhost:27017")
            self.database = self.client[DB]
            self.collection = self.database[COL]

            self.collection.create_index("animal_type")
            self.collection.create_index("breed")
            self.collection.create_index("name")

        except Exception as e:
            print(f"Database connection error: {e}")

    # ---------------------------
    # CREATE method
    # ---------------------------
    def create(self, data):
        if not isinstance(data, dict) or not data:
            raise ValueError("Invalid input: data must be a non-empty dictionary")

        # Basic validation (example required fields)
        required_fields = ["animal_type", "breed", "name"]
        for field in required_fields:
            if field not in data:
                raise ValueError(f"Missing required field: {field}")

        try:
            result = self.collection.insert_one(data)
            return result.acknowledged
        except Exception as e:
            print(f"Create error: {e}")
            return False

    # ---------------------------
    # READ method
    # ---------------------------
    def read(self, query):
        if not isinstance(query, dict):
            raise ValueError("Query must be a dictionary")

        try:
            results = list(self.collection.find(query))
            return results
        except Exception as e:
            print(f"Read error: {e}")
            return []

    # ---------------------------
    # UPDATE method
    # ---------------------------
    def update(self, query, update_data):
        if not isinstance(query, dict) or not isinstance(update_data, dict):
            raise ValueError("Query and update_data must be dictionaries")

        if not update_data:
            raise ValueError("update_data cannot be empty")

        try:
            result = self.collection.update_many(query, {"$set": update_data})
            return result.modified_count
        except Exception as e:
            print(f"Update error: {e}")
            return 0

    # ---------------------------
    # DELETE method
    # ---------------------------
    def delete(self, query):
        if not isinstance(query, dict):
            raise ValueError("Query must be a dictionary")

        try:
            result = self.collection.delete_many(query)
            return result.deleted_count
        except Exception as e:
            print(f"Delete error: {e}")
            return 0

if __name__ == "__main__":
    shelter = AnimalShelter()

    # READ existing records
    results = shelter.read({})
    print(f"Initial records found: {len(results)}")

    # CREATE
    test_dog = {
        "animal_type": "Dog",
        "breed": "Test Breed",
        "name": "Test Dog"
    }
    created = shelter.create(test_dog)
    print(f"Create successful: {created}")

    # READ inserted record
    test_results = shelter.read({"name": "Test Dog"})
    print(f"Inserted test records found: {len(test_results)}")

    # UPDATE
    updated = shelter.update({"name": "Test Dog"}, {"breed": "Updated Breed"})
    print(f"Records updated: {updated}")

    # DELETE
    deleted = shelter.delete({"name": "Test Dog"})
    print(f"Records deleted: {deleted}")