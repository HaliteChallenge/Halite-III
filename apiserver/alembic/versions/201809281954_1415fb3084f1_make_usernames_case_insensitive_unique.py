"""Make usernames case-insensitive unique

Revision ID: 1415fb3084f1
Revises: a37aed225eff
Create Date: 2018-09-28 19:54:51.798510+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '1415fb3084f1'
down_revision = 'a37aed225eff'
branch_labels = None
depends_on = None


def upgrade():
    op.drop_constraint("uq_user_username", "user")
    op.create_index("uq_user_username", "user", [sa.text("lower(username)")])


def downgrade():
    op.drop_index("uq_user_username", "user")
    op.create_unique_constraint("uq_user_username", "user", ["username"])
