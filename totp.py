import base64
import hashlib
import hmac
import struct
import time


def generate_totp(email):
    secret = email + "HENNGECHALLENGE004"

    # RFC 6238 normally expects a binary shared secret.
    # Encode the challenge secret as ASCII bytes.
    secret_bytes = secret.encode("ascii")

    time_step = 30
    current_time = int(time.time())
    counter = current_time // time_step

    # Convert the counter to an 8-byte big-endian value.
    counter_bytes = struct.pack(">Q", counter)

    digest = hmac.new(
        secret_bytes,
        counter_bytes,
        hashlib.sha512
    ).digest()

    # Dynamic truncation from RFC 4226.
    offset = digest[-1] & 0x0F

    binary_code = (
        ((digest[offset] & 0x7F) << 24)
        | ((digest[offset + 1] & 0xFF) << 16)
        | ((digest[offset + 2] & 0xFF) << 8)
        | (digest[offset + 3] & 0xFF)
    )

    otp = binary_code % (10 ** 10)

    # Keep leading zeroes so the result is always 10 digits.
    return str(otp).zfill(10)


def main():
    email = input("Email: ").strip()
    totp = generate_totp(email)

    print("TOTP:", totp)


if __name__ == "__main__":
    main()
