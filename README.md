# Urban-Farm Greenhouse Hybrid Energy Harvesting & Power Conditioning System

A real-time, modular energy-harvesting and climate-monitoring system for a grid-connected urban greenhouse. This project recovers low-grade agricultural waste heat using **Thermoelectric Generators (TEGs)** alongside **Solar PV** panels, regulates energy into an **Energy Storage System (ESS)**, and automates grid failover.

## UN SDG Alignment
* **SDG 7:** Affordable and Clean Energy
* **SDG 11:** Sustainable Cities and Communities

## Repository Files
* `greenhouse_node.ino` — Arduino firmware for dual-input MPPT PWM control, INA219 power sensing, and solid-state relay failover.
* `rpi_server.py` — Raspberry Pi supervisory controller and live web dashboard (24-hour India climate simulation).
* `dashboard.html` — Standalone browser-based real-time simulation interface.
* `Urban_Greenhouse_Hybrid_Energy_Project_Report.docx` — Complete project report, system architecture, and 24-hour telemetry logs.
