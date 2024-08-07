from importlib import import_module
import python.version as version

python_kyber = import_module(f"python_kyber{version.KYBER_VERSION}")
from python.helpers import transpose


def compress_decompress(p):
    if type(p).__name__ == "Polyvec":
        return python_kyber.Polyvec.from_bytes_compressed(p.to_bytes_compressed())
    elif type(p).__name__ == "Poly":
        return python_kyber.Poly.from_bytes_compressed(p.to_bytes_compressed())
    raise ValueError("{} it not a poly or a polyvec".format(type(p)))


def calc_delta_u(sample):
    r = sample.r.ntt()
    u = r.apply_matrix_left_ntt(transpose(sample.pk.a)).intt() + sample.e1
    u = u.reduce()
    u_uncompressed = u
    u = compress_decompress(u)
    deltau = u - u_uncompressed
    assert sample.ct.b.to_lists() == u.to_lists()
    return deltau


def calc_delta_v(sample):
    v = python_kyber.Polyvec.scalar(sample.pk.pk, sample.r.ntt()).intt() + sample.e2
    v2 = python_kyber.Polyvec.scalar(sample.pk.pk, sample.r.ntt()).intt() + sample.e2
    v = v + python_kyber.Poly.from_msg(sample.nu)
    v = v.reduce()
    v_uncompressed = v
    v = compress_decompress(v)
    deltav = v - v_uncompressed
    return deltav


def calc_error_term(sample):
    msg = sample.get_msg()
    su = python_kyber.Polyvec.scalar(sample.sk.sk, sample.ct.b.ntt()).intt()
    rec = (sample.ct.v - su).reduce()
    assert sample.nu == rec.to_msg()
    r = rec - msg
    return r, rec

# def calc_error_term(sample):
#     msg = sample.get_msg()
#     su = python_kyber.Polyvec.scalar(sample.sk.sk, sample.ct.b.ntt()).intt()
#     rec = (sample.ct.v - su).reduce()
#     # probably save this out here...
#     assert sample.nu == rec.to_msg()
#     r = rec - msg
#     print("R     =", r.to_bytes_uncompressed(), len(r.to_bytes_uncompressed()))
#     print("Rec   =", rec.to_bytes_uncompressed(), len(rec.to_bytes_uncompressed()))
#     print("Sample=", sample.nu, len(sample.nu))
#     print("Msg   =", msg.to_bytes_uncompressed(), len(msg.to_bytes_uncompressed()))
#     return r, rec
