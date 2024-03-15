def write_data(file_name: str, data: bytes) -> None:
    import base64

    if type(data) == bytes:
        # bytes to base64
        data = base64.b64encode(data)

    with open(file_name, 'wb+') as f:
        f.write(data)


def read_data(file_name: str) -> bytes:
    import base64

    with open(file_name, "rb") as f:
        data = f.read()
    # base64 to bytes
    return base64.b64decode(data)


def setupTenseal():
    '''Client-Side'''
    import tenseal as ts

    context = ts.context(ts.SCHEME_TYPE.CKKS, poly_modulus_degree=8192,
                         coeff_mod_bit_sizes=[60, 40, 40, 60])
    context.generate_galois_keys()
    context.global_scale = 2**40

    write_data("data/secret.txt", context.serialize(save_secret_key=True))

    context.make_context_public()
    write_data("data/public.txt", context.serialize())


def processImage() -> None:
    '''Server-Side'''
    import numpy as np
    from os import listdir
    from PIL import Image, UnidentifiedImageError
    import tenseal as ts

    context = ts.context_from(read_data("data/public.txt"))
    enc_v1 = ts.ckks_vector_from(context, read_data("data/enc_v.txt"))  # Get user's public key

    for filename in listdir("data"):
        print(filename)
        try:
            img = Image.open("data/"+filename)
            enc_v2 = ts.ckks_vector(context, np.array(
                img.resize((1832, 1920))).flatten())  # Encrypt comparison image with user's public key
            euclidean_squared = enc_v1 - enc_v2
            euclidean_squared = euclidean_squared.dot(euclidean_squared)  # Compare the encrypted images
            write_data("data/comp/"+filename+".txt",
                       euclidean_squared.serialize())  # Store comparison value
        except (UnidentifiedImageError, IsADirectoryError):
            pass


def encryptImg(src: str = "data/bedroom.jpg") -> None:
    '''Client-Side'''
    import numpy as np
    from os import listdir
    from PIL import Image
    import tenseal as ts

    img = Image.open(src)
    write_data("data/enc_v.txt", ts.ckks_vector(ts.context_from(read_data("data/public.txt")),
               np.array(img.resize((1832, 1920))).flatten()).serialize())  # Encrypt the user's image with public key

    processImage()  # Have server run detection on encrypted image from user

    context = ts.context_from(read_data("data/secret.txt"))

    for filename in listdir("data/comp"):  # Iterate Comparison values given by server
        euclidean_squared = ts.ckks_vector_from(
            context, read_data("data/comp/"+filename))
        print(f"{filename}: {euclidean_squared.decrypt()[0]}")  # Print decrypted comparison values


# setupTenseal()
encryptImg()
