# Artifact 3 – MongoDB Animal Shelter Dashboard

## Overview
This project is a full-stack data dashboard built using Python, MongoDB, and Dash. It allows users to interactively explore animal shelter data through filtering, visualization, and mapping.

Originally developed in CS-340, this artifact was enhanced to demonstrate real-world database integration and dynamic data visualization.

---

## Original Functionality
The original application included:
- Basic MongoDB CRUD operations
- A dashboard built with JupyterDash
- Predefined filter buttons
- Static data display

---

## Enhancements

### 1. Local MongoDB Integration
The application was migrated from a remote environment to a local MongoDB instance, improving:
- Accessibility
- Performance
- Real-world applicability

---

### 2. Improved CRUD Module
The `AnimalShelter` class was enhanced with:
- Create, Read, Update, Delete functionality
- Error handling
- Input validation

---

### 3. Dynamic Filtering System
Replaced static buttons with dropdown filters:
- Animal type
- Breed
- Outcome type

Queries are now built dynamically and executed against MongoDB.

---

### 4. Interactive Dashboard
The dashboard now includes:
- Data table with sorting and filtering
- Interactive map showing animal locations
- Pie chart visualization of breed distribution

All components update in real time based on user input.

---

## Skills Demonstrated
- Database design (MongoDB)
- Full-stack development
- API-style CRUD operations
- Data visualization with Dash and Plotly

---

## How to Run

### Requirements
- Python 3.x
- MongoDB installed and running

### Steps
1. Start MongoDB service
2. Import dataset using `mongoimport`
3. Run:
   ```bash
   python dashboard.py
