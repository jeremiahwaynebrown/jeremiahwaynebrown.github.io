# =========================
# Imports
# =========================
from dash import Dash
import dash
import dash_leaflet as dl
from dash import dcc, html, dash_table
from dash.dependencies import Input, Output
import plotly.express as px
import pandas as pd

from animal_shelter import AnimalShelter


# =========================
# Data Layer (Model)
# =========================

# Initialize connection to MongoDB via CRUD module
shelter = AnimalShelter()

# Retrieve all records from the database
df = pd.DataFrame.from_records(shelter.read({}))

# Remove MongoDB ObjectId field (not compatible with Dash DataTable)
if '_id' in df.columns:
    df.drop(columns=['_id'], inplace=True)

# Generate dropdown filter options dynamically from dataset
animal_type_options = sorted(df["animal_type"].dropna().unique()) if "animal_type" in df.columns else []
breed_options = sorted(df["breed"].dropna().unique()) if "breed" in df.columns else []
outcome_options = sorted(df["outcome_type"].dropna().unique()) if "outcome_type" in df.columns else []


# =========================
# Application Initialization
# =========================
app = Dash(__name__)


# =========================
# Layout (View)
# =========================
app.layout = html.Div([
    # ---------- Header Section ----------
    html.Center([
        html.A(
            html.Img(
                src='assets/Grazioso Salvare Logo.png',
                style={'width': '150px', 'marginBottom': '10px'}
            ),
            href='https://www.snhu.edu'
        ),
        html.H1('Grazioso Salvare Animal Rescue Dashboard')
    ]),
    html.Hr(),

    # ---------- Filter Controls ----------
    html.Div([
        dcc.Dropdown(
            id="filter-animal-type",
            options=[{"label": x, "value": x} for x in animal_type_options],
            placeholder="Filter by Animal Type",
            clearable=True
        ),
        dcc.Dropdown(
            id="filter-breed",
            options=[{"label": x, "value": x} for x in breed_options],
            placeholder="Filter by Breed",
            clearable=True
        ),
        dcc.Dropdown(
            id="filter-outcome",
            options=[{"label": x, "value": x} for x in outcome_options],
            placeholder="Filter by Outcome Type",
            clearable=True
        ),
        html.Button("Reset Filters", id="btn-reset", n_clicks=0)
    ], style={
        "display": "grid",
        "gridTemplateColumns": "1fr 1fr 1fr auto",
        "gap": "10px",
        "margin": "10px"
    }),

    # ---------- Data Table ----------
    dash_table.DataTable(
        id='datatable-id',
        columns=[{"name": i, "id": i} for i in df.columns],
        data=df.to_dict('records'),
        page_size=10,
        filter_action="native",
        sort_action="native",
        row_selectable="single",
        selected_rows=[0],
        style_table={'overflowX': 'auto'},
        style_cell={'textAlign': 'left', 'padding': '5px'},
        style_header={'backgroundColor': 'rgb(230, 230, 230)', 'fontWeight': 'bold'}
    ),

    html.Br(),

    # ---------- Map Visualization ----------
    html.Div(id='map-id'),

    html.Br(),

    # ---------- Pie Chart Visualization ----------
    dcc.Graph(id='pie-chart'),

    # ---------- Footer ----------
    html.Div(
        "Jeremy Brown | SNHU CS-340 MongoDB Dashboard",
        style={"marginTop": 20, "fontSize": 14, "color": "gray", "textAlign": "center"}
    )
])


# =========================
# Controller Logic (Callbacks)
# =========================

@app.callback(
    Output('datatable-id', 'data'),
    Output('filter-animal-type', 'value'),
    Output('filter-breed', 'value'),
    Output('filter-outcome', 'value'),
    Input('filter-animal-type', 'value'),
    Input('filter-breed', 'value'),
    Input('filter-outcome', 'value'),
    Input('btn-reset', 'n_clicks')
)
def update_table(animal_type, breed, outcome_type, reset_clicks):
    """
    Updates the data table based on selected filter values.

    If reset button is clicked:
        - Clears all filters
        - Reloads full dataset

    Otherwise:
        - Builds a dynamic MongoDB query based on selected filters
        - Returns filtered dataset
    """
    ctx = dash.callback_context

    # Reset filters and reload all data
    if ctx.triggered and ctx.triggered[0]['prop_id'].split('.')[0] == 'btn-reset':
        dff = pd.DataFrame.from_records(shelter.read({}))
        if '_id' in dff.columns:
            dff.drop(columns=['_id'], inplace=True)
        return dff.to_dict('records'), None, None, None

    # Build dynamic query
    query = {}

    if animal_type:
        query["animal_type"] = animal_type
    if breed:
        query["breed"] = breed
    if outcome_type:
        query["outcome_type"] = outcome_type

    # Execute query
    dff = pd.DataFrame.from_records(shelter.read(query))

    if '_id' in dff.columns:
        dff.drop(columns=['_id'], inplace=True)

    return dff.to_dict('records'), animal_type, breed, outcome_type


@app.callback(
    Output('map-id', "children"),
    Input('datatable-id', "derived_virtual_data"),
    Input('datatable-id', "derived_virtual_selected_rows")
)
def update_map(viewData, index):
    """
    Updates map visualization based on selected row in the table.

    Displays:
        - Geographic location (latitude/longitude)
        - Marker with tooltip and popup info
    """
    if not viewData or not index:
        return [html.Div("No data available")]

    dff = pd.DataFrame.from_dict(viewData)
    row = index[0]

    try:
        lat = dff.at[row, 'location_lat']
        lon = dff.at[row, 'location_long']
        breed = dff.at[row, 'breed'] if 'breed' in dff.columns else ''
        name = dff.at[row, 'name'] if 'name' in dff.columns else ''
    except Exception:
        return [html.Div("Invalid or missing location data")]

    return [
        dl.Map(
            style={'width': '1000px', 'height': '500px'},
            center=[lat, lon],
            zoom=10,
            children=[
                dl.TileLayer(id='base-layer-id'),
                dl.Marker(
                    position=[lat, lon],
                    children=[
                        dl.Tooltip(breed),
                        dl.Popup([
                            html.H1("Animal Name"),
                            html.P(name)
                        ])
                    ]
                )
            ]
        )
    ]


@app.callback(
    Output('pie-chart', 'figure'),
    Input('datatable-id', 'data')
)
def update_pie(viewData):
    """
    Updates pie chart visualization based on filtered table data.

    Displays:
        - Distribution of animal breeds
    """
    dff = pd.DataFrame.from_dict(viewData)

    if 'breed' not in dff.columns or dff.empty:
        return px.pie(values=[1], names=["No Data"], title="Breed Distribution")

    fig = px.pie(
        dff,
        names='breed',
        title="Breed Distribution",
        width=800,
        height=800
    )

    fig.update_layout(
        legend=dict(
            orientation="v",
            x=1.05,
            y=0.5,
            font=dict(size=12),
            bgcolor="rgba(0,0,0,0)"
        )
    )

    return fig


# =========================
# Application Entry Point
# =========================
if __name__ == "__main__":
    app.run(debug=True)