import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports

class SerialConnectionApp(tk.Toplevel):
    def __init__(self, root):
        super().__init__(root)
        self.title("Serial Connection")
        self.configure(background="#1E1E1E")
        self.geometry("400x250")

        tk.Frame(self, height=25, bg="#54AEFF").pack(fill="x", pady=0)

        self.serial_settings = {}
        self.serial_connection = None
        
        self.create_widgets()
        self.refresh_ports()

    def create_widgets(self):
        frame = tk.Frame(self, bg="#1E1E1E")
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tk.Label(frame, text="Select COM Port:           ", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=0, column=0, sticky="w")
        self.port_combobox = ttk.Combobox(frame, width=15)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Baudrate:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=1, column=0, sticky="w")
        self.baudrate_combobox = ttk.Combobox(frame, values=["9600", "14400", "19200", "38400", "57600", "115200"], width=15)
        self.baudrate_combobox.set("9600")
        self.baudrate_combobox.grid(row=1, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Parity:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=2, column=0, sticky="w")
        self.parity_combobox = ttk.Combobox(frame, values=["N", "E", "O", "M", "S"], width=15)
        self.parity_combobox.set("N")
        self.parity_combobox.grid(row=2, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Data bits:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=3, column=0, sticky="w")
        self.databits_combobox = ttk.Combobox(frame, values=[5, 6, 7, 8], width=15)
        self.databits_combobox.set(8)
        self.databits_combobox.grid(row=3, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Stopbits:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=3, column=0, sticky="w")
        self.stopbits_combobox = ttk.Combobox(frame, values=[1, 2], width=15)
        self.stopbits_combobox.set(1)
        self.stopbits_combobox.grid(row=4, column=1, padx=5, pady=5, sticky="ew")

        frame.columnconfigure(1, weight=1)

        button_frame = tk.Frame(self, bg="#1E1E1E")
        button_frame.pack(fill="x", padx=10, pady=10)

        self.save_button = tk.Button(button_frame, text="Save", command=self.save_settings, bg="#54AEFF", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.save_button.pack(side="right", expand=False, padx=5)

        self.cancel_button = tk.Button(button_frame, text="Cancel", command=self.destroy, bg="#383c42", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.cancel_button.pack(side="right", expand=False, padx=5)

        self.refresh_button = tk.Button(button_frame, text="Refresh", command=self.refresh_ports, bg="#383c42", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.refresh_button.pack(side="right", expand=False, padx=5)

              

    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [port.device for port in ports]
        self.port_combobox['values'] = port_list

    def save_settings(self):
        self.serial_settings["port"] = self.port_combobox.get()
        self.serial_settings["baudrate"] = self.baudrate_combobox.get()
        self.serial_settings["parity"] = self.parity_combobox.get()
        self.serial_settings["stopbits"] = self.stopbits_combobox.get()
        messagebox.showinfo("Settings Saved", "Connection settings saved successfully.")
        self.destroy()

class MainApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Main Application")
        self.geometry("300x200")
        self.configure(background="#1E1E1E")

        tk.Label(self, text="Main Application", bg="#1E1E1E", fg="white", font=("Arial", 14)).pack(pady=20)
        open_serial_button = tk.Button(self, text="Open Serial Connection", command=self.open_serial, bg="#54AEFF", fg="white", width=20)
        open_serial_button.pack(pady=10)

    def open_serial(self):
        SerialConnectionApp(self)

if __name__ == "__main__":
    app = MainApp()
    app.mainloop()
