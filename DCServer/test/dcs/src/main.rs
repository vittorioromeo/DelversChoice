use std::io;
use std::net; 
use std::vec;

macro_rules! force
{
    ($expr:expr, $error:expr) => 
    (
        $expr.ok().expect($error);
    )
}

fn read_line_and_parse<T>() -> T
    where T: std::str::FromStr
{
    let mut input = String::new();
    
    force!(io::stdin().read_line(&mut input), 
        "Failed to read line.");

    force!(input.trim().parse(),
        "Failed to parse.")
}

type Port = u16;

fn choice_server() -> Result<String, Box<std::error::Error>>
{
    println!("Insert receive port");
    let receive_port: Port = read_line_and_parse();

    // println!("Insert send port");
    // let send_port: Port = read_line_and_parse();

    let server_ip = std::net::Ipv4Addr::new(127, 0, 0, 1);
    
    let mut server_socket = force!(std::net::UdpSocket::bind((server_ip, receive_port)),
        "Failed binding socket.");

    let mut buf: [u8; 2048] = [0; 2048];

    loop 
    {
        println!("Waiting for data");
        let (amt, src) = try!(server_socket.recv_from(&mut buf));

        println!("Got data");
        println!("Bytes: {}", amt);
        println!("From: {}", src);
        println!("Content: {}", String::from_utf8_lossy(&buf[0..amt]));
    }
}

fn choice_client() -> Result<String, Box<std::error::Error>>
{
    let client_ip = std::net::Ipv4Addr::new(127, 0, 0, 1);

    println!("Insert send ip");
    let server_ip: std::net::Ipv4Addr = read_line_and_parse(); 

    println!("Insert send port");
    let send_port: Port = read_line_and_parse();

    println!("Insert receive port");
    let receive_port: Port = read_line_and_parse();

    let mut client_socket = try!(std::net::UdpSocket::bind((client_ip, receive_port)));

    let mut buf: [u8; 2048] = [0; 2048];
    buf[0] = 'a' as u8;

    loop 
    {
        println!("Sending data");
        let amt = try!(client_socket.send_to(&buf[0..1], (server_ip, send_port)));

        println!("Sent data");
        println!("Bytes: {}", amt);
        println!("Content: {}", String::from_utf8_lossy(&buf[0..amt]));
    }
}  

fn main()
{
    println!("0. Server");
    println!("1. Client");
    println!("_. Exit");

    match read_line_and_parse()
    {
        0 => choice_server().ok(),
        1 => choice_client().ok(),
        _ => std::process::exit(0)
    };
}

